#pragma once

#include "vulkan_types.inl"

void VulkanCreateRenderpass(
    VulkanContext *context,
    VulkanRenderpass *outRenderpass,
    f32 x, f32 y, f32 w, f32 h,
    f32 r, f32 g, f32 b, f32 a,
    f32 depth,
    u32 stencil);

void VulkanDestroyRenderpass(
    VulkanContext *context,
    VulkanRenderpass *renderpass);

void VulkanBeginRenderpass(
    VulkanCommandBuffer *commandBuffer,
    VulkanRenderpass *renderpass,
    VkFramebuffer frameBuffer);

void VulkanEndRenderpass(
  VulkanCommandBuffer *commandBuffer,
  VulkanRenderpass *renderpass);
