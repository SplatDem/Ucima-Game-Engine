#include "vulkan_fence.h"
#include "defines.h"
#include <stdio.h>

void VulkanCreateFence(
    VulkanContext *context,
    BOOLEAN createSignaled,
    VulkanFence *outFence) 
{
  outFence->isSignaled = createSignaled;
  VkFenceCreateInfo fenceCreateInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
  if (outFence->isSignaled)
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  VK_CHECK(vkCreateFence(context->device.device, &fenceCreateInfo, context->allocator, &outFence->handle));
}

void VulkanDestroyFence(VulkanContext *context, VulkanFence *fence) {
  if (fence->handle) {
    vkDestroyFence(context->device.device, fence->handle, context->allocator);
    fence->handle = 0;
  }
  fence->isSignaled = FALSE;
}

BOOLEAN VulkanWaitFence(VulkanContext *context, VulkanFence *fence, u64 timeoutNanoSecs) {
  if (!fence->isSignaled) {
    VkResult result = vkWaitForFences(
        context->device.device,
        1,
        &fence->handle,
        TRUE,
        timeoutNanoSecs);
    switch (result) {
      case VK_SUCCESS: 
        fence->isSignaled = TRUE; 
        break;
      case VK_TIMEOUT:
        S_TraceLogWarn("VkWaitFence - Time out");
        break;
      case VK_ERROR_DEVICE_LOST:
        S_TraceLogError("VkWaitFence - VK_ERROR_DEVICE_LOST");
        break;
      case VK_ERROR_OUT_OF_HOST_MEMORY:
        S_TraceLogError("VkWaitFence - VK_ERROR_OUT_OF_HOST_MEMORY");
        break;
      case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        S_TraceLogError("VkWaitFence - VK_ERROR_OUT_OF_DEVICE_MEMORY");
        break;
      default:
        S_TraceLogError("VkWaitFence - Unknown error");
        break;
    }
  } else return TRUE;
  return FALSE;
}

void VulkanResetFence(VulkanContext *context, VulkanFence *fence) {
  if (fence->isSignaled) {
    VK_CHECK(vkResetFences(context->device.device, 1, &fence->handle));
    fence->isSignaled = FALSE;
  }
}
