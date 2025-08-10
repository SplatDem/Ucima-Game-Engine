#include "clock.h"
#include "platform/platform.h"

void UpdateClock(Clock *clock) {
  if (clock->startTime != 0)
    clock->elapsed = PlatformGetClockTime() - clock->startTime;
}

void StartClock(Clock *clock) {
  clock->startTime = PlatformGetClockTime();
  clock->elapsed = 0;
}

void StopClock(Clock *clock) {
  clock->startTime = 0;
}
