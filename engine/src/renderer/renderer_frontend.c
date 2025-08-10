#include "renderer/renderer_types.inl"
#include "renderer_backend.h"
#include "renderer_frontend.h"
#include "core/umemory.h"
#include "defines.h"
#include <stdio.h>

// Backend renderer context
static RendererBackend *backend = 0;

BOOLEAN InitRenderer(const char *appName, struct PlatformState *pState) {
  backend = ualloc(sizeof(RendererBackend), MEMORY_CATEGORY_RENDERER);
  
  InitRendererBackend(RENDERER_BACKEND_TYPE_VULKAN, pState, backend);
  backend->frameNumber = 0;

  if (!backend->Init(backend, appName, pState)) {
    S_TraceLogError("Failed to init renderer backend");
  }

  return TRUE;
}

void DestroyRenderer() {
  backend->Destroy(backend);
  ufree(backend, sizeof(RendererBackend), MEMORY_CATEGORY_RENDERER);
}

b8 RendererBeginFrame(f32 deltaTime) {
  return backend->BeginFrame(backend, deltaTime);
}

b8 RendererEndFrame(f32 deltaTime) {
  b8 result = backend->EndFrame(backend, deltaTime);
  backend->frameNumber++;
  return result;
}

BOOLEAN RendererDrawFrame(RenderPacket *packet) {
  if (RendererBeginFrame(packet->deltaTime)) {
    b8 result = RendererEndFrame(packet->deltaTime);

    if (!result) {
      S_TraceLogError("Failed to call RendererEndFrame");
      return FALSE;
    }
  }
  return TRUE;
}
