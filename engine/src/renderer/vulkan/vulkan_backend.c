#include "vulkan_backend.h"
#include "defines.h"
#include "core/ustring.h"
#include "vulkan_swapchain.h"
#include "vulkan_types.inl"
#include "vulkan_device.h"
#include "containers/darray.h"
#include "vulkan_platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

static VulkanContext context;

#define MAX_REQUIRED_EXTENSIONS 16

i32 FindMemoryIndex(u32 typeFilter, u32 propertyFlags);

BOOLEAN InitVulkanRendererBackend(RendererBackend* backend, const char* appName, struct PlatformState* pState) {
  RUNTIMEMESSAGE("\x1b[35mIMPORTANT TODO: Use dynamic arrays\x1b[0m");

  context.FindMemoryIndex = FindMemoryIndex;

  RUNTIMEMESSAGE("TODO: Custom vulkan allocator");
  context.allocator = 0;

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

  // Swapchain
  VulkanCreateSwapchain(
      &context,
      context.framebufferWidth,
      context.framebufferHeight,
      &context.swapchain);

  S_TraceLogDebug("Vulkan renderer initialized successfully");
  return TRUE;
}

void DestroyVulkanRendererBackend(RendererBackend *backend) {
  RUNTIMEMESSAGE("TODO: Destroy debugger");
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
