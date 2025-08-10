#include "application.h"
#include "core/umemory.h"
#include "game_types.h"
#include <platform/platform.h>
#include "core/event.h"
#include "core/input.h"
#include "clock.h"
#include <defines.h>
#include <stdio.h>

#include "renderer/renderer_frontend.h"
#include "renderer/renderer_types.inl"

typedef struct AppState {
  Game *gameInst;
  BOOLEAN running;
  BOOLEAN suspend;
  PlatformState platform;
  s16 width;
  s16 height;
  Clock clock;
  f64 lastTime;
} AppState;

static BOOLEAN initialized = FALSE;
static AppState appState;

BOOLEAN appOnEvent(u16 code, void *sender, void *listenerInst, EventContext context);
BOOLEAN appOnKey(u16 code, void *sender, void *listenerInst, EventContext context);

BOOLEAN AppCreate(Game *gameInst) {
  if (initialized) {
    S_TraceLogError("Can't call application more then once");
    return FALSE;
  }
  
  appState.gameInst = gameInst;

  InitInput();

  appState.running = TRUE;
  appState.suspend = FALSE;

  if (!InitEvent()) {
    S_TraceLogError("Failed to init event system");
    return FALSE;
  }

  if (!PlatformStartUp(
        &appState.platform, 
        gameInst->appConfig.name, 
        gameInst->appConfig.startPosX, 
        gameInst->appConfig.startPosY,
        gameInst->appConfig.startWidth, 
        gameInst->appConfig.startHeight
  )) 
  {
    return FALSE;
  }

  if (!InitRenderer(gameInst->appConfig.name, &appState.platform)) {
    S_TraceLogError("Failed to initialize renderer");
    return FALSE;
  }

  RegisterEvent(EVENT_CODE_APPLICATION_QUIT, 0, appOnEvent);
  RegisterEvent(EVENT_CODE_KEY_PRESSED, 0, appOnEvent);
  RegisterEvent(EVENT_CODE_KEY_RELEASED, 0, appOnEvent);

  if (!appState.gameInst->initialize(appState.gameInst)) {
    S_TraceLogError("Failed to initialize game");
    return FALSE;
  }

  appState.gameInst->onResize(appState.gameInst, appState.width, appState.height);

  initialized = TRUE;

  return TRUE;
}

BOOLEAN AppRun() {
  StartClock(&appState.clock);
  UpdateClock(&appState.clock);
  appState.lastTime = appState.clock.elapsed;
  f64 runningTime = 0;
  u8 frameCount = 0;
  f64 targetFrameSecs = 1.0f / 60;

  S_TraceLogInfo("%s", DEBUG_GetMemUsageStr());
  while (appState.running) {
    if(!PlatformPollEvent(&appState.platform)) appState.running = FALSE;

    if (!appState.suspend) {
      UpdateClock(&appState.clock);
      f64 currentTime = appState.clock.elapsed;
      f64 delta = (currentTime - appState.lastTime);
      f64 frameStartTime = PlatformGetClockTime();

      if (!appState.gameInst->update(appState.gameInst, (f32)delta)) {
        S_TraceLogError("Failed to update game. Shutting down :(");
        appState.running = FALSE;
        break;
      }

      if (!appState.gameInst->render(appState.gameInst, (f32)delta)) {
        S_TraceLogError("Failed to render game. Shutting down :(");
        appState.running = FALSE;
        break;
      }

      // TODO: Change this
      RenderPacket packet;
      packet.deltaTime = delta;
      RendererDrawFrame(&packet);

      f64 frameEndTime = PlatformGetClockTime();
      f64 frameElapsedTime = frameEndTime - frameStartTime;
      runningTime += frameElapsedTime;
      f64 remainingSecs = targetFrameSecs - frameElapsedTime;

      if (remainingSecs > 0) {
        u64 remainingMs = (remainingSecs * 1000);
        BOOLEAN limitFrames = FALSE;
        if (remainingMs > 0 && limitFrames)
          PlatformSleep(remainingMs - 1);
        frameCount++;
      }

      // NOTE: Input update happen only at the end of the 
      // current frame and the start of the next frame.
      // Input update/state copying should always be handled
      // after any input shpuld be recorded.
      UpdateInput(delta);

      appState.lastTime = currentTime;
    }
  }
  appState.running = FALSE;
  UnregisterEvent(EVENT_CODE_APPLICATION_QUIT, 0, appOnEvent);
  UnregisterEvent(EVENT_CODE_KEY_PRESSED, 0, appOnEvent);
  UnregisterEvent(EVENT_CODE_KEY_RELEASED, 0, appOnEvent);
  DestroyEvent();
  DestroyInput();
  DestroyRenderer();
  PlatformShutDown(&appState.platform);
  return TRUE;
}

// TODO: make callback function
BOOLEAN appOnEvent(u16 code, void *sender, void *listenerInst, EventContext context) {
  switch (code) {
    case EVENT_CODE_APPLICATION_QUIT: {
      S_TraceLogInfo("Quit event recieved");
      appState.running = FALSE;
      return TRUE;
    }
  }
  return FALSE;
}

// TODO: make callback function
BOOLEAN appOnKey(u16 code, void *sender, void *listenerInst, EventContext context) {
  if (code == EVENT_CODE_KEY_PRESSED) {
    u16 keyCode = context.data.u16[0];
    if (keyCode == KEY_ESCAPE) {
      EventContext data = {};
      FireEvent(EVENT_CODE_APPLICATION_QUIT, 0, data);
      return TRUE;
    } else {
      S_TraceLogInfo("'%c' key pressed in window", keyCode);
    }
  } else if (code == EVENT_CODE_KEY_RELEASED) {
    u16 keyCode = context.data.u16[0];
    S_TraceLogInfo("'%c' key released in window", keyCode);
  }

  return FALSE;
}
