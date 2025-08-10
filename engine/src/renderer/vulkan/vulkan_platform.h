#pragma once

#include "defines.h"

struct PlatformState;
struct VulkanContext;

BOOLEAN PlatformCreateVulkanSurface(struct PlatformState *pState, struct VulkanContext *context);
void PlatformGetRequiredExtensionNames(const char ***names);
