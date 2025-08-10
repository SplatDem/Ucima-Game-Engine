#pragma once

#include "renderer_types.inl"

struct StaticMeshData;
struct PlatformState;

b8 InitRenderer(const char *appName, struct PlatformState *pState);
void DestroyRenderer();
void RendererOnResize(u16 width, u16 height);
b8 RendererDrawFrame(RenderPacket *packet);
