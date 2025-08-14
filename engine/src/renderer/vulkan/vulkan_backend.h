#pragma once

#include "platform/platform.h"
#include "renderer/renderer_types.inl"

BOOLEAN InitVulkanRendererBackend(RendererBackend *backend, const char *appName, struct PlatformState *pState);
void DestroyVulkanRendererBackend(RendererBackend *backend);
void VulkanRendererOnResize(RendererBackend *backend, u16 width, u16 height);
BOOLEAN VulkanRendererBackendBeginFrame(RendererBackend *backend, f32 deltaTime);
BOOLEAN VulkanRendererBackendEndFrame(RendererBackend *backend, f32 deltaTime);
