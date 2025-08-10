#pragma once

#include "defines.h"

typedef struct Clock {
  f64 startTime;
  f64 elapsed;
} Clock;

void UpdateClock(Clock *clock);
void StartClock(Clock *clock);
void StopClock(Clock *clock);
