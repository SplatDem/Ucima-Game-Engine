#pragma once

#include <core/application.h>
#include <defines.h>
#include <stdio.h>
#include "game_types.h"
#include "core/umemory.h"

extern b8 CreateGame(Game *outGame);

// Application entry point
i32 main() {
  InitMemory();
  Game gameInst;
  if (!CreateGame(&gameInst)) {
    S_TraceLogError("Failed to create a game");
    return -1;
  }

  if (!gameInst.render      ||
      !gameInst.update      ||
      !gameInst.initialize  ||
      !gameInst.onResize)
  {
    S_TraceLogError("The game function pointers must be assigned");
    return -2;
  }

  if(!AppCreate(&gameInst)) {
    S_TraceLogError("Failed to create application");
    return 1;
  }

  if(!AppRun()) {
    S_TraceLogError("Failed to run application");
    return 2;
  }

  DestroyMemory();
  return 0;
}
