#pragma once

#include "defines.h"
#include "core/asserts.h"

#include "vulkan/vulkan.h"

#define VK_CHECK(expr) \
  { \
    UASSERT(expr == VK_SUCCESS); \
  }

//       context->device.physicalDevice = physicalDevices[i];
//       context->device.graphicsQueueIndex = queueInfo.graphicsFamilyIndex;
//       context->device.presentQueueIndex = queueInfo.presentFamilyIndex;
//       context->device.transferQueueIndex = queueInfo.transferFamilyIndex;
// 
//       context->device.properties = properties;
//       context->device.features = features;
//       context->device.memory = memory;

typedef struct VulkanSwapchainSupportInfo {
  VkSurfaceCapabilitiesKHR capabilities;
  u32 formatCount;
  VkSurfaceFormatKHR *formats;
  u32 presentModeCount;
  VkPresentModeKHR *presentMode;
} VulkanSwapchainSupportInfo;

typedef struct VulkanDevice {
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VulkanSwapchainSupportInfo swapchainSupport;
  s32 graphicsQueueIndex;
  s32 presentQueueIndex;
  s32 transferQueueIndex;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  VkQueue transferQueue;
  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceFeatures features;
  VkPhysicalDeviceMemoryProperties memory;
} VulkanDevice;

typedef struct VulkanContext {
  // Custom Vulkan allocator
  VkAllocationCallbacks *allocator;

  VkInstance instance;
  VkSurfaceKHR surface;

  // Both physical and logical devices
  VulkanDevice device;
} VulkanContext;
