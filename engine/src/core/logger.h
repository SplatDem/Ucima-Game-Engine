#pragma once

#include "defines.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#if URELEASE == 1
  #define LOG_LEVEL_DEBUG_ENABLED 0
  #define LOG_LEVEL_TRACE_ENABLED 0
#endif

typedef enum LogLevel {
  LOG_LEVEL_FATAL = 0, // App will crash
  LOG_LEVEL_ERROR, // App probably will crash
  LOG_LEVEL_WARN, // App more likly don't crash
  LOG_LEVEL_INFO, // Info
  LOG_LEVEL_DEBUG, // Debug message if debug mode enabled
  LOG_LEVEL_TRACE, // Verbose version of debug message
} LogLevel;

char InitLogging();
void DestroyLogging();

UAPI void LogMessage(LogLevel level, const char *message, ...);

#define LFATAL(message, ...) LogMessage(LOG_LEVEL_FATAL, message, ##__VA_AGRS__)

#ifdef LERROR
  #define LERROR(message, ...) LogMessage(LOG_LEVEL_ERROR, message, ##__VA_AGRS__)
#endif

#if LOG_WARN_ENABLED == 1
  #define LWARN(message, ...) LogMessage(LOG_LEVEL_WARN, message, ##__VA_AGRS__)
#else
  #define LWARN(message, ...)
#endif

#define LINFO(message, ...) LogMessage(LOG_LEVEL_INFO, message, ##__VA_AGRS__)

#if LOG_TRACE_ENABLED == 1
  #define LTRACE(message, ...) LogMessage(LOG_LEVEL_TRACE, message, ##__VA_AGRS__)
#else
  #define LTRACE(message, ...)
#endif

