#pragma once

#include "defines.h"
#include "core/asserts.h"

#include "vulkan/vulkan.h"

#define VK_CHECK(expr) \
  { \
    UASSERT(expr == VK_SUCCESS); \
  }

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
  VkFormat depthFormat;
} VulkanDevice;

typedef struct VulkanImage {
  VkImage handle;
  VkDeviceMemory memory;
  VkImageView view;
  u32 width;
  u32 height;
} VulkanImage;

typedef struct VulkanSwapchain {
  VkSurfaceFormatKHR imageFormat;
  u8 maxFramesInFlight;
  VkSwapchainKHR handle;
  u32 imageCount;
  VkImage *images;
  VkImageView *views;
  VulkanImage depthAttachment;
} VulkanSwapchain;

typedef struct VulkanContext {
  // Custom Vulkan allocator
  VkAllocationCallbacks *allocator;

  VkInstance instance;
  VkSurfaceKHR surface;

  // Both physical and logical devices
  VulkanDevice device;

  VulkanSwapchain swapchain;
  u32 imageIndex;
  u32 currentFrame;
  BOOLEAN recreatingSwapchain;

  // Framebuffer size for swapchain
  u32 framebufferWidth;
  u32 framebufferHeight;

  i32 (*FindMemoryIndex)(u32 typeFilter, u32 propertyFlags);
} VulkanContext;
