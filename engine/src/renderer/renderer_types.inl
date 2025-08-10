#pragma once

#include "defines.h"

typedef enum RendererBackendType {
  RENDERER_BACKEND_TYPE_VULKAN,
  RENDERER_BACKEND_TYPE_OPENGL,
  RENDERER_BACKEND_TYPE_DIRECTX,
  RENDERER_BACKEND_TYPE_METAL
} RendererBackendType;

typedef struct RendererBackend {
  struct PlatformState *pState;
  u64 frameNumber;

  b8 (*Init)(struct RendererBackend *backend, const char *appName, struct PlatformState *pState);
  void (*Destroy)(struct RendererBackend *backend);
  void (*Resize)(struct RendererBackend *backend, u16 width, u16 height);
  b8 (*BeginFrame)(struct RendererBackend *backend, f32 deltaTime); // Just like raylib
  b8 (*EndFrame)(struct RendererBackend *backend, f32 deltaTime);
} RendererBackend;

typedef struct RenderPacket {
  f64 deltaTime;
} RenderPacket;
