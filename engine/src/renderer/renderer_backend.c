#include "renderer_backend.h"
#include "renderer/vulkan/vulkan_backend.h"
#include "defines.h"
#include <stdio.h>

BOOLEAN InitRendererBackend(
    RendererBackendType type,
    struct PlatformState *pState,
    RendererBackend *outRendererBackend)
{
  outRendererBackend->pState = pState;

  if (type == RENDERER_BACKEND_TYPE_VULKAN) {
    outRendererBackend->Init = InitVulkanRendererBackend;
    outRendererBackend->Destroy = DestroyVulkanRendererBackend;
    outRendererBackend->BeginFrame = VulkanRendererBackendBeginFrame;
    outRendererBackend->EndFrame = VulkanRendererBackendEndFrame;
    outRendererBackend->Resize = VulkanRendererOnResize;
    return TRUE;
  }

  S_TraceLogError("Unknown renderer backend");
  return FALSE;
}

void DestroyRendererBackend(RendererBackend *rendererBackend) {
  rendererBackend->Init = 0;
  rendererBackend->Destroy = 0;
  rendererBackend->BeginFrame = 0;
  rendererBackend->EndFrame = 0;
  rendererBackend->Resize = 0;
}
