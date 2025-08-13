#pragma once

#include "vulkan_types.inl"

void VulkanCreateFramebuffer(
    VulkanContext *context,
    VulkanRenderpass *renderpass,
    u32 width,
    u32 height,
    u32 attachmentCount,
    VkImageView *attachments,
    VulkanFramebuffer *outFramebuffer);

void VulkanDestroyFramebuffer(VulkanContext *context, VulkanFramebuffer *framebuffer);
