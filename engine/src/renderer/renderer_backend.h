#pragma once

#include "renderer_types.inl"

struct PlatformState;

b8 InitRendererBackend(RendererBackendType type, struct PlatformState *pState, RendererBackend *outRendererBackend);
void DestroyRendererBackend(RendererBackend *rendererBackend);
