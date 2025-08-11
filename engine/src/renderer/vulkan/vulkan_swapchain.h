#pragma once

#include "vulkan_types.inl"

void VulkanCreateSwapchain(
    VulkanContext *context,
    u32 width,
    u32 height,
    VulkanSwapchain *swapchain);

void VulkanRecreateSwapchain(
    VulkanContext *context,
    u32 width,
    u32 height,
    VulkanSwapchain *swapchain);

void VulkanDestroySwapchain(
    VulkanContext *context,
    VulkanSwapchain *swapchain);

BOOLEAN VulkanSwapchainAcquireNextImageIndex(
    VulkanContext *context,
    VulkanSwapchain *swapchain,
    u64 timeoutNanoSecs,
    VkSemaphore imageAvailableSemaphore,
    VkFence fence,
    u32 *outImageIndex);

void VulkanSwapchainPresent(
    VulkanContext *context,
    VulkanSwapchain *swapchain,
    VkQueue graphicsQueue,
    VkQueue presentQueue,
    VkSemaphore renderCompleteSemaphore,
    u32 presentImageIndex);
