#include "defines.h"
#include "containers/darray.h"
#include "core/umemory.h"
#include "core/ustring.h"
#include "core/application.h"

#include "vulkan_backend.h"
#include "vulkan_platform.h"
#include "vulkan_swapchain.h"
#include "vulkan_types.inl"
#include "vulkan_device.h"
#include "vulkan_renderpass.h"
#include "vulkan_command_buffer.h"
#include "vulkan_framebuffer.h"
#include "vulkan_fence.h"

#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

static VulkanContext context;
static u32 cachedFramebufferWidth = 0;
static u32 cachedFramebufferHeight = 0;

#define MAX_REQUIRED_EXTENSIONS 16

i32 FindMemoryIndex(u32 typeFilter, u32 propertyFlags);

void CreateCommandBuffer(RendererBackend *backend);
void RegenerateFramebuffer(RendererBackend *backend, VulkanSwapchain *swapchain, VulkanRenderpass *renderpass);

BOOLEAN InitVulkanRendererBackend(RendererBackend* backend, const char* appName, struct PlatformState* pState) {
  RUNTIMEMESSAGE("\x1b[35mIMPORTANT TODO: Use dynamic arrays\x1b[0m");

  context.FindMemoryIndex = FindMemoryIndex;

  RUNTIMEMESSAGE("TODO: Custom vulkan allocator");
  context.allocator = 0;

  AppGetFramebufferSize(&cachedFramebufferWidth, &cachedFramebufferHeight);
  context.framebufferWidth = (cachedFramebufferWidth != 0) ? cachedFramebufferWidth : 900;
  context.framebufferHeight = (cachedFramebufferHeight != 0) ? cachedFramebufferHeight : 800;
  cachedFramebufferWidth = 0;
  cachedFramebufferHeight = 0;

  VkApplicationInfo appInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
  appInfo.apiVersion = VK_API_VERSION_1_2;
  appInfo.pApplicationName = appName;
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Ucima Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

  VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  createInfo.pApplicationInfo = &appInfo;

  const char* requiredExtensions[MAX_REQUIRED_EXTENSIONS] = {0};
  u32 requiredExtensionCount = 0;

  requiredExtensions[requiredExtensionCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
  
  RUNTIMEMESSAGE("\x1b[31mULTRA FUCKING BIG KOSTIL HERE BLYAT. FIX THIS, SUKA OR DIIIIIIIIIIIEEEEEEEEEEEEE\x1b[0m");
  const char* platformExtensions[8] = {0};
  u32 platformExtensionCount = 0;
    
#if defined (UPLATFORM_LINUX)
    platformExtensions[0] = "VK_KHR_xcb_surface";
    platformExtensionCount++;
#endif

#if defined (UPLATFORM_WINDOWS)
    platformExtensions[0] = "VK_KHR_win32_surface";
    platformExtensionCount++;
#endif

  for (u32 i = 0; i < platformExtensionCount && requiredExtensionCount < 16; i++)
      requiredExtensions[requiredExtensionCount++] = platformExtensions[i];

#if defined(_DEBUG)
    requiredExtensions[requiredExtensionCount++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

    S_TraceLogDebug("Required extensions:");
    for (u32 i = 0; i < requiredExtensionCount; ++i) {
        S_TraceLogDebug("%s", requiredExtensions[i]);
    }
#endif

  createInfo.enabledExtensionCount = requiredExtensionCount;
  createInfo.ppEnabledExtensionNames = requiredExtensions;

  RUNTIMEMESSAGE("\x1b[31mETO PROSTO PIZDEC BLYAT\x1b[0m");
  // Validation layers
  const char* requiredValidationLayerNames[] = {
      "VK_LAYER_KHRONOS_validation"
  };
  u32 requiredValidationLayersCount = 0;

#if defined(_DEBUG)
    S_TraceLogDebug("Validation layers enabled. Enumerating...");
    requiredValidationLayersCount = 1;  // Only using one layer

    // Verify layers are available
    u32 availableLayerCount = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, 0));
    VkLayerProperties* availableLayers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * availableLayerCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers));

    for (u32 i = 0; i < requiredValidationLayersCount; ++i) {
        S_TraceLogDebug("Searching for layer: %s...", requiredValidationLayerNames[i]);
        BOOLEAN found = FALSE;
        for (u32 j = 0; j < availableLayerCount; ++j) {
            if (ustrcmp(requiredValidationLayerNames[i], availableLayers[j].layerName) == 0) {
                found = TRUE;
                S_TraceLogDebug("Found");
                break;
            }
        }
        if (!found) {
            S_TraceLogError("Missing required validation layer: %s", requiredValidationLayerNames[i]);
            free(availableLayers);
            return FALSE;
        }
    }
    free(availableLayers);
    S_TraceLogDebug("All required validation layers are present");
#endif

  createInfo.enabledLayerCount = requiredValidationLayersCount;
  createInfo.ppEnabledLayerNames = requiredValidationLayerNames;

  VK_CHECK(vkCreateInstance(&createInfo, context.allocator, &context.instance));
  S_TraceLogDebug("Vulkan instance created successfully");

#if defined (_DEBUG)
    RUNTIMEMESSAGE("TODO: Vulkan debugger (#014)");
#endif

  // Surface
  S_TraceLogDebug("Creating vulkan surface");
  if (!PlatformCreateVulkanSurface(pState, &context)) {
    S_TraceLogError("Failed to create vulkan surface");
    return FALSE;
  }
  S_TraceLogDebug("Vulkan surface created successfully");

  // Device
  if (!VulkanCreateDevice(&context)) {
    S_TraceLogError("Failed to create vulkan device");
    return FALSE;
  }
  S_TraceLogDebug("Vulkan logical device created successfully");

  // Swapchain
  VulkanCreateSwapchain(
      &context,
      context.framebufferWidth,
      context.framebufferHeight,
      &context.swapchain);
  S_TraceLogDebug("Vulkan swapchain created successfully");

  // Starting from x = 0; y = 0;
  VulkanCreateRenderpass(
      &context,
      &context.mainRenderpass,
      0, 0, context.framebufferWidth, context.framebufferHeight,
      0.0f, 0.0f, 0.2f, 1.0f,
      1.0f,
      0);
  S_TraceLogDebug("Vulkan renderpass created successfully");

  // Swapchain framebuffers
  context.swapchain.framebuffers = da_reserve(VulkanFramebuffer, context.swapchain.imageCount);
  RegenerateFramebuffer(backend, &context.swapchain, &context.mainRenderpass);
  S_TraceLogDebug("Vulkan framebuffers created successfully");

  CreateCommandBuffer(backend);
  S_TraceLogDebug("Vulkan buffers created successfully");

  S_TraceLogDebug("Creating sync objects...");
  context.imageAvailableSemaphores = da_reserve(VkSemaphore, context.swapchain.maxFramesInFlight);
  context.queueCompleteSemaphores = da_reserve(VkSemaphore, context.swapchain.maxFramesInFlight);
  context.inFlightFences = da_reserve(VkSemaphore, context.swapchain.maxFramesInFlight);

  for (u32 i = 0; i < context.swapchain.maxFramesInFlight; ++i) {
    VkSemaphoreCreateInfo semaphoreCreateInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    vkCreateSemaphore(context.device.device, &semaphoreCreateInfo, context.allocator, &context.imageAvailableSemaphores[i]);
    vkCreateSemaphore(context.device.device, &semaphoreCreateInfo, context.allocator, &context.queueCompleteSemaphores[i]);
    VulkanCreateFence(&context, TRUE, &context.inFlightFences[i]);

    context.imagesInFlight = da_reserve(VulkanFence, context.swapchain.imageCount);
    for (u32 i = 0; i < context.swapchain.imageCount; ++i)
      context.imagesInFlight[i] = 0;
  }

  S_TraceLogDebug("Vulkan renderer initialized successfully");
  return TRUE;
}

void DestroyVulkanRendererBackend(RendererBackend *backend) {
  vkDeviceWaitIdle(context.device.device);

  RUNTIMEMESSAGE("\x1b[45mTODO: Destroy semaphores properly\x1b[0m");
  for (u8 i = 0; i < context.swapchain.maxFramesInFlight; ++i) {
//     if (context.imageAvailableSemaphores[i]) {
//       ERRORLETTER
//       vkDestroySemaphore(
//           context.device.device,
//           context.imageAvailableSemaphores[i],
//           context.allocator);
//       context.imageAvailableSemaphores[i] = 0;
//     }
    if (context.queueCompleteSemaphores[i]) {
      vkDestroySemaphore(
          context.device.device,
          context.queueCompleteSemaphores[i],
          context.allocator);
      context.queueCompleteSemaphores[i] = 0;
    }
    VulkanDestroyFence(&context, &context.inFlightFences[i]);
  }
//   da_destroy(context.imageAvailableSemaphores);
//   context.imageAvailableSemaphores = 0;
//   da_destroy(context.queueCompleteSemaphores);
//   context.queueCompleteSemaphores = 0;
//   da_destroy(context.inFlightFences);
//   context.inFlightFences = 0;
//   da_destroy(context.imagesInFlight);
//   context.imagesInFlight = 0;

  for (u32 i = 0; i < context.swapchain.imageCount; ++i) {
    if (context.graphicsCommandBuffers[i].handle) {
      VulkanFreeCommandBuffer(
          &context,
          context.device.graphicsCommandPool,
          &context.graphicsCommandBuffers[i]);
      context.graphicsCommandBuffers[i].handle = 0;
    }
  }
  da_destroy(context.graphicsCommandBuffers);
  context.graphicsCommandBuffers = 0;

  for (u32 i = 0; i < context.swapchain.imageCount; ++i)
    VulkanDestroyFramebuffer(&context, &context.swapchain.framebuffers[i]);
  VulkanDestroyRenderpass(&context, &context.mainRenderpass);
  VulkanDestroySwapchain(&context, &context.swapchain);
  VulkanDestroyDevice(&context);
  vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
  vkDestroyInstance(context.instance, context.allocator);
}

void VulkanRendererOnResize(RendererBackend *backend, u16 width, u16 height) {}

BOOLEAN VulkanRendererBeginFrame(RendererBackend *backend, f32 deltaTime) {
  return TRUE;
}

BOOLEAN VulkanRendererEndFrame(RendererBackend *backend, f32 deltaTime) {
  return TRUE;
}

i32 FindMemoryIndex(u32 typeFilter, u32 propertyFlags) {
  VkPhysicalDeviceMemoryProperties memProp;
  vkGetPhysicalDeviceMemoryProperties(context.device.physicalDevice, &memProp);

  for (u32 i = 0; i < memProp.memoryTypeCount; ++i) {
    if (typeFilter & (1 << i) && (memProp.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)
      return i;
  }

  S_TraceLogWarn("Failed to find suitable memory type");
  return -1;
}

void CreateCommandBuffer(RendererBackend *backend) {
  if (!context.graphicsCommandBuffers) {
    context.graphicsCommandBuffers = da_reserve(VulkanCommandBuffer, context.swapchain.imageCount);
    for (u32 i = 0; i < context.swapchain.imageCount; ++i)
      uzeromem(&context.graphicsCommandBuffers[i], sizeof(VulkanCommandBuffer));
  }

  for (u32 i = 0; i < context.swapchain.imageCount; ++i) {
    if (context.graphicsCommandBuffers[i].handle)
      VulkanFreeCommandBuffer(&context, context.device.graphicsCommandPool, &context.graphicsCommandBuffers[i]);
    uzeromem(&context.graphicsCommandBuffers[i], sizeof(VulkanCommandBuffer));
    VulkanAllocateCommandBuffer(&context, context.device.graphicsCommandPool, TRUE, &context.graphicsCommandBuffers[i]);
  }
}

void RegenerateFramebuffer(RendererBackend *backend, VulkanSwapchain *swapchain, VulkanRenderpass *renderpass) {
  for (u32 i = 0; i < swapchain->imageCount; ++i) {
    u32 attachmentCount = 2;
    VkImageView attachments[] = {
      swapchain->views[i],
      swapchain->depthAttachment.view,
    };
    VulkanCreateFramebuffer(
        &context,
        renderpass,
        context.framebufferWidth,
        context.framebufferHeight,
        attachmentCount,
        attachments,
        &context.swapchain.framebuffers[i]);
  }
}
