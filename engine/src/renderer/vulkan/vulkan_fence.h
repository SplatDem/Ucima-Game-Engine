#pragma once

#include "vulkan_types.inl"

void VulkanCreateFence(
    VulkanContext *context,
    BOOLEAN createSingled,
    VulkanFence *outFence);

void VulkanDestroyFence(VulkanContext *context, VulkanFence *fence);

BOOLEAN VulkanWaitFence(VulkanContext *context, VulkanFence *fence, u64 timeoutNanoSecs);

void VulkanResetFence(VulkanContext *context, VulkanFence *fence);
