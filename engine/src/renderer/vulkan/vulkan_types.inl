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
  VkCommandPool graphicsCommandPool;
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

typedef enum VulkanRenderpassState {
  READY,
  RECORDING,
  IN_RENDER_PASS,
  RECORDING_ENDED,
  SUBMITTED,
  NOT_ALLOCATED
} VulkanRenderpassState;

typedef struct VulkanRenderpass {
  VkRenderPass handle;
  f32 x, y, w, h;
  f32 r, g, b, a;
  f32 depth;
  u32 stencil;
  VulkanRenderpassState state;
} VulkanRenderpass;

typedef struct VulkanFramebuffer {
  VkFramebuffer handle;
  u32 attachmentCount;
  VkImageView *attachments;
  VulkanRenderpass *renderpass;
} VulkanFramebuffer;

typedef struct VulkanSwapchain {
  VkSurfaceFormatKHR imageFormat;
  u8 maxFramesInFlight;
  VkSwapchainKHR handle;
  u32 imageCount;
  VkImage *images;
  VkImageView *views;
  VulkanImage depthAttachment;
  VulkanFramebuffer *framebuffers;
} VulkanSwapchain;

typedef enum VulkanCommandBufferState {
  COMMAND_BUFFER_STATE_READY,
  COMMAND_BUFFER_STATE_RECORDING,
  COMMAND_BUFFER_STATE_IN_RENDER_PASS,
  COMMAND_BUFFER_STATE_RECORDING_ENDED,
  COMMAND_BUFFER_STATE_SUBMITTED,
  COMMAND_BUFFER_STATE_NOT_ALLOCATED
} VulkanCommandBufferState;

typedef struct VulkanCommandBuffer {
  VkCommandBuffer handle;
  VulkanCommandBufferState state;
} VulkanCommandBuffer;

typedef struct VulkanFence {
  VkFence handle;
  BOOLEAN isSignaled;
} VulkanFence;

typedef struct VulkanContext {
  // Custom Vulkan allocator
  VkAllocationCallbacks *allocator;

  VkInstance instance;
  VkSurfaceKHR surface;

  // Both physical and logical devices
  VulkanDevice device;

  // Swapchain and other image processing
  VulkanSwapchain swapchain;

  u32 inFlightFenceCount;
  VkSemaphore *imageAvailableSemaphores;
  VkSemaphore *queueCompleteSemaphores;
  VulkanFence *inFlightFences;
  VulkanFence **imagesInFlight;
  u32 imageIndex;
  u32 currentFrame;
  BOOLEAN recreatingSwapchain;

  VulkanRenderpass mainRenderpass;

  VulkanCommandBuffer *graphicsCommandBuffers;

  // Framebuffer size for swapchain
  u32 framebufferWidth;
  u32 framebufferHeight;

  u64 framebufferSizeGeneration;
  u64 framebufferSizeLastGeneration;

  i32 (*FindMemoryIndex)(u32 typeFilter, u32 propertyFlags);
} VulkanContext;
