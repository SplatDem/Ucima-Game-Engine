#pragma once

#include "platform/platform.h"
#include "renderer/renderer_types.inl"

BOOLEAN InitOpenGLRendererBackend(RendererBackend *backend, const char *appName, struct PlatformState *pState);
void DestroyOpenGLRendererBackend(RendererBackend *backend);
void OpenGLRendererOnResize(RendererBackend *backend, u16 width, u16 height);
BOOLEAN OpenGLRendererBackendBeginFrame(RendererBackend *backend, f32 deltaTime);
BOOLEAN OpenGLRendererBackendEndFrame(RendererBackend *backend, f32 deltaTime);
