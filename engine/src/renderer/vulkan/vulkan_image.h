#pragma once

#include "vulkan_types.inl"

void VulkanCreateImage(
    VulkanContext *context,
    i32 imageType,
    u32 width,
    u32 height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memoryFlags,
    i32 createView,
    VkImageAspectFlags viewAspectFlags,
    VulkanImage *outImage);

void VulkanCreateImageView(
    VulkanContext *context,
    VkFormat format,
    VulkanImage *image,
    VkImageAspectFlags aspectFlags);

void VulkanDestroyImage(VulkanContext *context, VulkanImage *image);
