#include "vulkan_image.h"
#include "defines.h"
#include "renderer/vulkan/vulkan_types.inl"
#include "vulkan_device.h"
#include "core/umemory.h"
#include <stdio.h>

void VulkanCreateImage(
    VulkanContext *context,
    i32 imageType,
    u32 width,
    u32 height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memoryFlags,
    i32 createView,
    VkImageAspectFlags viewAspectFlags,
    VulkanImage *outImage)
{
  outImage->width = width;
  outImage->height = height;

  #define ICI imageCreateInfo

  VkImageCreateInfo imageCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
  ICI.imageType = VK_IMAGE_TYPE_2D;
  ICI.extent.width = width;
  ICI.extent.height = height;
  ICI.extent.depth = 1; // TODO: Configurable depth
  ICI.mipLevels = 4; // TODO: Mip mapping
  ICI.arrayLayers = 1; // Number of layer in the image
  ICI.format = format;
  ICI.tiling = tiling;
  ICI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  ICI.usage = usage;
  ICI.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Configurable sample count
  ICI.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Configurable sharing mode

  VK_CHECK(vkCreateImage(context->device.device, &imageCreateInfo, context->allocator, &outImage->handle));

  VkMemoryRequirements memoryRequirements;
  vkGetImageMemoryRequirements(context->device.device, outImage->handle, &memoryRequirements);

  i32 memoryType = context->FindMemoryIndex(memoryRequirements.memoryTypeBits, memoryFlags);
  if (memoryType == -1)
    S_TraceLogError("Invalid image: required memory type not found");

  VkMemoryAllocateInfo memAllocInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
  memAllocInfo.allocationSize = memoryRequirements.size;
  memAllocInfo.memoryTypeIndex = memoryType;
  VK_CHECK(vkAllocateMemory(context->device.device, &memAllocInfo, context->allocator, &outImage->memory));
  VK_CHECK(vkBindImageMemory(context->device.device, outImage->handle, outImage->memory, 0)); // TODO: Configurable memory offset

  if (createView) {
    outImage->view = 0;
    VulkanCreateImageView(
        context,
        format,
        outImage,
        viewAspectFlags);
  }
}

void VulkanCreateImageView(
    VulkanContext *context,
    VkFormat format,
    VulkanImage *image,
    VkImageAspectFlags aspectFlags)
{
  #define VCI viewCreateInfo
  #define VCIS viewCreateInfo.subresourceRange

  VkImageViewCreateInfo viewCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
  VCI.image = image->handle;
  VCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
  VCI.format = format;
  VCIS.aspectMask = aspectFlags;

  // TODO: Make configurable
  VCIS.baseMipLevel = 0;
  VCIS.levelCount = 1;
  VCIS.baseArrayLayer = 0;
  VCIS.layerCount = 1;

  VK_CHECK(vkCreateImageView(context->device.device, &viewCreateInfo, context->allocator, &image->view));
}

void VulkanDestroyImage(VulkanContext *context, VulkanImage *image) {
  if (image->view) {
    vkDestroyImageView(context->device.device, image->view, context->allocator);
    image->view = 0;
  }
  if (image->memory) {
    vkFreeMemory(context->device.device, image->memory, context->allocator);
    image->memory = 0;
  }
  if (image->handle) {
    vkDestroyImage(context->device.device, image->handle, context->allocator);
    image->handle = 0;
  }
}
