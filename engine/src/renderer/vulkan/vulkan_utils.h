#pragma once
#include "vulkan_types.inl"

BOOLEAN VkResultIsSuccess(VkResult result);
const char *VkResultToString(VkResult result, BOOLEAN verbose);
