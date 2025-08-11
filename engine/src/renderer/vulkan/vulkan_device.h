#pragma once

#include "vulkan_types.inl"

BOOLEAN VulkanCreateDevice(VulkanContext *context);
void VulkanDestroyDevice(VulkanContext *context);

void VulkanDeviceQuerySwapchainSupport(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VulkanSwapchainSupportInfo *outSupportInfo);

BOOLEAN VulkanDeviceDetectDepthFormat(VulkanDevice *device);
