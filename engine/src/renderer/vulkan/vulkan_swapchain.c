#include "defines.h"
#include "core/umemory.h"
#include "renderer/vulkan/vulkan_types.inl"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_image.h"
#include <stdio.h>
#include <vulkan/vulkan_core.h>

void create(
    VulkanContext *context,
    u32 width,
    u32 height,
    VulkanSwapchain *swapchain);

void destroy(VulkanContext *context, VulkanSwapchain *swapchain);

void VulkanCreateSwapchain(
    VulkanContext *context,
    u32 width,
    u32 height,
    VulkanSwapchain *swapchain)
{
  create(context, width, height, swapchain);
}

void VulkanRecreateSwapchain(
    VulkanContext *context,
    u32 width,
    u32 height,
    VulkanSwapchain *swapchain)
{
  destroy(context, swapchain);
  create(context, width, height, swapchain);
}

void VulkanDestroySwapchain(
    VulkanContext *context,
    VulkanSwapchain *swapchain)
{
  destroy(context, swapchain);
}

// Semaphores uses to syncronize the GPU operation between other GPU operaions
// Fences uses to syncronize operation between the application and GPU
BOOLEAN VulkanSwapchainAcquireNextImageIndex(
    VulkanContext *context,
    VulkanSwapchain *swapchain,
    u64 timeoutNanoSecs,
    VkSemaphore imageAvailableSemaphore,
    VkFence fence,
    u32 *outImageIndex)
{
  VkResult result = vkAcquireNextImageKHR(
      context->device.device,
      swapchain->handle,
      timeoutNanoSecs,
      imageAvailableSemaphore,
      fence,
      outImageIndex);


  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    VulkanRecreateSwapchain(context, context->framebufferWidth, context->framebufferHeight, swapchain);
    return FALSE;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    S_TraceLogError("Failed to acquire swapchain image");
    return FALSE;
  }

  return TRUE;
}

void VulkanSwapchainPresent(
    VulkanContext *context,
    VulkanSwapchain *swapchain,
    VkQueue graphicsQueue,
    VkQueue presentQueue,
    VkSemaphore renderCompleteSemaphore,
    u32 presentImageIndex)
{
  // Return the image to the swapchain presentation
  VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &renderCompleteSemaphore;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &swapchain->handle;
  presentInfo.pImageIndices = &presentImageIndex;
  presentInfo.pResults = 0;

  VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    VulkanRecreateSwapchain(context, context->framebufferWidth, context->framebufferHeight, swapchain);
  else if (result != VK_SUCCESS)
    S_TraceLogError("Failed to present swapchain image");

  context->currentFrame = (context->currentFrame + 1) % swapchain->maxFramesInFlight;
}

void create(
    VulkanContext *context,
    u32 width,
    u32 height,
    VulkanSwapchain *swapchain)
{
  // Some sort of a swapchain size
  VkExtent2D swapchainExtent = {width, height};
  swapchain->maxFramesInFlight = 2;

  BOOLEAN found = FALSE;
  for (u32 i = 0; i < context->device.swapchainSupport.formatCount; ++i) {
    VkSurfaceFormatKHR format = context->device.swapchainSupport.formats[i];
    // Preferred formats that every GPU should support
    if (format.format == VK_FORMAT_B8G8R8_UNORM
        && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      swapchain->imageFormat = format;
      found = TRUE;
      break;
    }
  }

  if (!found)
    swapchain->imageFormat = context->device.swapchainSupport.formats[0];

  VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
  for (u32 i = 0; i < context->device.swapchainSupport.presentModeCount; ++i) {
    VkPresentModeKHR mode = context->device.swapchainSupport.presentMode[i];
    if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
      presentMode = mode;
      break;
    }
  }

  VulkanDeviceQuerySwapchainSupport(
      context->device.physicalDevice,
      context->surface,
      &context->device.swapchainSupport);

  if (context->device.swapchainSupport.capabilities.currentExtent.width != UINT32_MAX)
    swapchainExtent = context->device.swapchainSupport.capabilities.currentExtent;

  VkExtent2D min = context->device.swapchainSupport.capabilities.minImageExtent;
  VkExtent2D max = context->device.swapchainSupport.capabilities.maxImageExtent;
  swapchainExtent.width = UCLAMP(swapchainExtent.width, min.width, max.width);
  swapchainExtent.height = UCLAMP(swapchainExtent.height, min.height, max.height);

  u32 imageCount = context->device.swapchainSupport.capabilities.minImageCount +1;
  if (context->device.swapchainSupport.capabilities.maxImageCount > 0 
      && imageCount > context->device.swapchainSupport.capabilities.maxImageCount) {
    imageCount = context->device.swapchainSupport.capabilities.maxImageCount;
  }

  #define SCI swapchainCreateInfo

  VkSwapchainCreateInfoKHR swapchainCreateInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
  SCI.surface = context->surface;
  SCI.minImageCount = imageCount;
  SCI.imageFormat = swapchain->imageFormat.format;
  SCI.imageColorSpace = swapchain->imageFormat.colorSpace;
  SCI.imageExtent = swapchainExtent;
  SCI.imageArrayLayers = 1;
  SCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  if (context->device.graphicsQueueIndex != context->device.presentQueueIndex) {
    u32 queueFamilyIndices[] = {
      (u32)context->device.graphicsQueueIndex,
      (u32)context->device.presentQueueIndex,
    };
    SCI.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    SCI.queueFamilyIndexCount = 2;
    SCI.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    SCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    SCI.queueFamilyIndexCount = 0;
    SCI.pQueueFamilyIndices = 0;
  }

  SCI.preTransform = context->device.swapchainSupport.capabilities.currentTransform;
  SCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  SCI.presentMode = presentMode;
  SCI.clipped = VK_TRUE;
  SCI.oldSwapchain = 0; // We are just destroying old swapchain and creating new one

  VK_CHECK(vkCreateSwapchainKHR(context->device.device, &swapchainCreateInfo, context->allocator, &swapchain->handle));

  context->currentFrame = 0;
  context->imageIndex = 0;
  VK_CHECK(vkGetSwapchainImagesKHR(context->device.device, swapchain->handle, &swapchain->imageCount, 0));
  if (!swapchain->images)
    swapchain->images = (VkImage*)ualloc(sizeof(VkImage) * swapchain->imageCount, MEMORY_CATEGORY_RENDERER);
  if (!swapchain->views)
    swapchain->views = (VkImageView*)ualloc(sizeof(VkImageView) * swapchain->imageCount, MEMORY_CATEGORY_RENDERER);
  VK_CHECK(vkGetSwapchainImagesKHR(context->device.device, swapchain->handle, &swapchain->imageCount, swapchain->images));

  for (u32 i = 0; i < swapchain->imageCount; ++i) {
    VkImageViewCreateInfo viewInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    viewInfo.image = swapchain->images[i];
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = swapchain->imageFormat.format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(context->device.device, &viewInfo, context->allocator, &swapchain->views[i]));
  }

  if (!VulkanDeviceDetectDepthFormat(&context->device)) {
    context->device.depthFormat = VK_FORMAT_UNDEFINED;
    S_TraceLogWarn("Failed to find a supported format");
  }

  VulkanCreateImage(
      context,
      VK_IMAGE_TYPE_2D,
      swapchainExtent.width,
      swapchainExtent.height,
      context->device.depthFormat,
      VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      TRUE,
      VK_IMAGE_ASPECT_DEPTH_BIT,
      &swapchain->depthAttachment);

  S_TraceLogInfo("Vulakn swapchain created successfuly");
}

void destroy(VulkanContext *context, VulkanSwapchain *swapchain) {
  VulkanDestroyImage(context, &swapchain->depthAttachment);
  for (u32 i = 0; i < swapchain->imageCount; ++i)
    vkDestroyImageView(context->device.device, swapchain->views[i], context->allocator);
  vkDestroySwapchainKHR(context->device.device, swapchain->handle, context->allocator);
}
