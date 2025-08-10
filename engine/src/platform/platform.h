// Crossplatform wrapper over WINAPI, X11, Wayland and probably MacOS thing (whatever it is)

#pragma once

#include "defines.h"

typedef struct PlatformState {
  void *internalState;
} PlatformState;

BOOLEAN PlatformStartUp(
    PlatformState *platformState,
    const char *appName,
    i32 x, i32 y, i32 width, i32 height);

void PlatformShutDown(PlatformState *platformState);

BOOLEAN PlatformPollEvent(PlatformState *platformState);

void *PlatformAllocate(u64 size, b8 aligned);
void PlatformFree(void *block, b8 aligned);
void *PlatformZeroMem(void *block, u64 size);
void *PlatformMemCopy(void *dest, const void *src, u64 size);
void *PlatformMemSet(void *dest, i32 value, u64 size);

void PlatformWriteConsole(const char *message, u8 color);
void PlatformWriteConsoleError(const char *message, u8 color);

f64 PlatformGetClockTime();
void PlatformSleep(u64 msecs);
