#pragma once

#include <defines.h>

struct Game;

typedef struct AppConfig {
  s16 startPosX;
  s16 startPosY;

  s16 startWidth;
  s16 startHeight;

  char *name;
} AppConfig;

UAPI BOOLEAN AppCreate(struct Game *gameInst);
UAPI BOOLEAN AppRun();
void AppGetFramebufferSize(u32 *width, u32 *height);
