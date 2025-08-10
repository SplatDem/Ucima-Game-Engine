#include "game.h"
#include <entry.h>
#include <defines.h>
#include <core/umemory.h>

b8 CreateGame(Game *game) {
  // App configuration
  game->appConfig.startPosX = 1920/2;
  game->appConfig.startPosY = 1080/2;
  game->appConfig.startWidth = 900;
  game->appConfig.startHeight = 800;
  game->appConfig.name = "Ucima engine Test app";

  game->update = GameUpdate;
  game->render = GameRender;
  game->initialize = GameInitialize;
  game->onResize = GameOnResize;

  game->state = ualloc(sizeof(GameState), MEMORY_CATEGORY_GAME);

  return TRUE;
}
