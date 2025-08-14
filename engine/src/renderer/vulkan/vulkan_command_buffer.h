#pragma once

#include "vulkan_types.inl"

// We allocating command buffer from command pool
void VulkanAllocateCommandBuffer(
    VulkanContext *context,
    VkCommandPool pool,
    BOOLEAN isPrimary,
    VulkanCommandBuffer *outCommandBuffer);

void VulkanFreeCommandBuffer(
    VulkanContext *context,
    VkCommandPool pool,
    VulkanCommandBuffer *commandBuffer);

void VulkanBeginCommandBuffer(
    VulkanCommandBuffer *commandBuffer,
    BOOLEAN isSingleuse,
    BOOLEAN isRenderpassContinue,
    BOOLEAN isSimultaneousUse);

void VulkanEndCommandBuffer(VulkanCommandBuffer *commandBuffer);

void VulkanCommandBufferUpdateSubmitted(VulkanCommandBuffer *commandBuffer);

void VulkanResetCommandBuffer(VulkanCommandBuffer *commandBuffer);
