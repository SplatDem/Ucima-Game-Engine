#pragma once

#include "defines.h"

#define UASSERTION_ENABLED

#if defined (UASSERTION_ENABLED)
  #if _MSC_VER
    #include <intrin.h>
    #define debugBreak() __debugbreak()
  #else
    #define debugBreak() __builtin_trap()
  #endif

void AssertionFailureReport(const char *expression, const char *message, const char *file, i32 line);

#define UASSERT(expr) \
  { \
    if (expr) {} \
    else { \
      AssertionFailureReport(#expr, "", __FILE__, __LINE__); \
      debugBreak(); \
    } \
  }

#define UASSERT_MSG(expr, message) \
  { \
    if (expr) {} \
    else { \
      AssertionFailureReport(#expr, message, __FILE__, __LINE__); \
      debugBreak(); \
    } \
  }

#else

#define UASSERT(expr)
#define UASSERT_MSG(expr, message)

#endif
