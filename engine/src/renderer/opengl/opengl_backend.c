#include "opengl_backend.h"

BOOLEAN InitOpenGLRendererBackend(RendererBackend *backend, const char *appName, struct PlatformState *pState) {
  return TRUE;
}

void DestroyOpenGLRendererBackend(RendererBackend *backend) {}

void OpenGLRendererOnResize(RendererBackend *backend, u16 width, u16 height) {}

BOOLEAN OpenGLRendererBackendBeginFrame(RendererBackend *backend, f32 deltaTime) {
  return TRUE;
}

BOOLEAN OpenGLRendererBackendEndFrame(RendererBackend *backend, f32 deltaTime) {
  return TRUE;
}
