#pragma once

#include <defines.h>
#include <core/application.h>

typedef struct Game {
  AppConfig appConfig;
  b8 (*initialize)(struct Game *gameInst);
  b8 (*update)(struct Game *gameInst, f32 deltaTime);
  b8 (*render)(struct Game *gameInst, f32 deltaTime);
  void (*onResize)(struct Game *gameInst, u32 width, u32 height);
  void *state;
} Game;
