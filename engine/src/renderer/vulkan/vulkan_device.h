#pragma once

#include "vulkan_types.inl"

BOOLEAN VulkanCreateDevice(VulkanContext *context);
void VulkanDestroyDevice(VulkanContext *context);

void VulkanDeviceSwapchainSupport(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VulkanSwapchainSupportInfo *outSupportInfo);
