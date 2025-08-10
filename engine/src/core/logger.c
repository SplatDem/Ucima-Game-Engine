#include "logger.h"
#include "core/asserts.h"
#include "defines.h"
#include <platform/platform.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define MAX_MESSAGE_LEN 32000

void AssertionFailureReport(const char *expression, const char *message, const char *file, i32 line) {
  S_TraceLogError("Assertion failed: %s, message: %s. (File: %s, Line: %d)", expression, message, file, line);
}

void LogMessage(LogLevel level, const char* message, ...) {
  const char *levelStr[6] = {"[\x1b[41mFATAL\x1b[0m]: ", "[\x1b[31mERROR\x1b[0m]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};
  b8 isError = level < 2;

  char outMessage[MAX_MESSAGE_LEN];
  memset(outMessage, MAX_MESSAGE_LEN, sizeof(outMessage));
    
  __builtin_va_list arg_ptr;
  va_start(arg_ptr, message);
  vsnprintf(outMessage, MAX_MESSAGE_LEN, message, arg_ptr);
  va_end(arg_ptr);

  char outMessage2[MAX_MESSAGE_LEN];
  memset(outMessage2, MAX_MESSAGE_LEN, sizeof(outMessage2));

  if (isError) PlatformWriteConsoleError(outMessage2, level);
  else PlatformWriteConsole(outMessage2, level);
}
