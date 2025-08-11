#ifndef STYPES_H
#define STYPES_H

#define TRUE 1
#define FALSE 0

#if defined (STYPES_STD_INT_UINT_T)

#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef char b8;

#else

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

typedef float f32;
typedef double f64;

typedef int i32;
typedef char b8;

#endif

#define BOOLEAN b8

#define INFO "[\x1b[34mINFO\x1b[0m]: "
#define ERROR "[\x1b[31mERROR\x1b[0m]: "
#define WARN "[\x1b[33mWARN\x1b[0m]: "
#define DEBUG "[\x1b[35mDEBUG\x1b[0m]: "
#define ERRORLETTER printf("\x1b[31me\x1b[0m (File: %s, Line: %d)\n", __FILE__, __LINE__);

#if defined (_DEBUG)
  #define RUNTIMEMESSAGE(fmt, ...) printf("\x1b[45mmessage\x1b[0m: " fmt " (File: %s, Line: %d)\n", ##__VA_ARGS__, __FILE__, __LINE__)
#else
  #define RUNTIMEMESSAGE(fmt, ...)
#endif

#define ARRAYLEN(array) sizeof(array)/sizeof(*array)

#if defined (_WIN32)
  #define S_TraceLogInfo(fmt, ...) printf(INFO fmt "\r\n", ##__VA_ARGS__)
#elif defined (linux)
  #define S_TraceLogInfo(fmt, ...) printf(INFO fmt "\n", ##__VA_ARGS__)
#else
  #define S_TraceLogInfo(fmt, ...) printf(INFO fmt "\n", ##__VA_ARGS__)
#endif

#if defined (_WIN32)
  #define S_TraceLogError(fmt, ...) printf(ERROR fmt "\r\n", ##__VA_ARGS__)
#elif defined (linux)
  #define S_TraceLogError(fmt, ...) printf(ERROR fmt "\n", ##__VA_ARGS__)
#else
  #define S_TraceLogError(fmt, ...) printf(ERROR fmt "\n\nPlatform doesnt supports properly!\n", ##__VA_ARGS__)
#endif

#if defined (_WIN32)
  #define S_TraceLogWarn(fmt, ...) printf(WARN fmt "\r\n", ##__VA_ARGS__)
#elif defined (linux)
  #define S_TraceLogWarn(fmt, ...) printf(WARN fmt "\n", ##__VA_ARGS__)
#else
  #define S_TraceLogWarn(fmt, ...) printf(WARN fmt "\n\nPlatform doesnt supports properly!\n", ##__VA_ARGS__)
#endif

#if defined  (_DEBUG)
  #if defined (_WIN32)
    #define S_TraceLogDebug(fmt, ...) printf(DEBUG fmt "\r\n", ##__VA_ARGS__)
  #elif defined (linux)
    #define S_TraceLogDebug(fmt, ...) printf(DEBUG fmt "\n", ##__VA_ARGS__)
  #else
    #define S_TraceLogDebug(fmt, ...) printf(DEBUG fmt "\n\nPlatform doesnt supports properly!\n", ##__VA_ARGS__)
  #endif
#else
  #if defined (_WIN32)
    #define S_TraceLogDebug(fmt, ...)
  #elif defined (linux)
    #define S_TraceLogDebug(fmt, ...)
  #else
    #define S_TraceLogDebug(fmt, ...)
  #endif
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#    define DPLATFORM_WINDOWS 1
#    ifndef _WIN64
#        error "64-bit is required on Windows!"
#    endif
#elif defined(__linux__) || defined(__gnu_linux__)
// Linux OS
#    define UPLATFORM_LINUX 1
#    if defined(__ANDROID__)
#        define UPLATFORM_ANDROID 1
#    endif
#elif defined(__unix__)
// Catch anything not caught by the above
#    define UPLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
// Posix
#    define UPLATFORM_POSIX 1
#elif __APPLE__
// Apple platforms
#    define UPLATFORM_APPLE 1
#    include <TargetConditionals.h>
#    if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#        define UPLATFORM_IOS 1
#        define UPLATFORM_IOS_SIMULATOR 1
#    elif TARGET_OS_IPHONE
#        define UPLATFORM_IOS 1
// iOS device
#    elif TARGET_OS_MAC
// HACK: Should probably be in the Vulkan Renderer lib, not here
#        define VK_USE_PLATFORM_MACOS_MVK
// Other kinds of Mac OS
#    else
#        error "Unknown Apple platform"
#    endif
#else
#    error "Unknown platform"
#endif

#ifdef UEXPORT
// Exports
#    ifdef _MSC_VER
#        define UAPI __declspec(dllexport)
#    else
#        define UAPI __attribute__((visibility("default")))
#    endif
#else
// Imports
#    ifdef _MSC_VER
#        define UAPI __declspec(dllimport)
#    else
#        define UAPI
#    endif
#endif

#if _DEBUG
#    define U_DEBUG 1
#    define U_RELEASE 0
#else
#    define U_RELEASE 1
#    define U_DEBUG 0
#endif

// Feature build flags
#if U_DEBUG
#    define U_HOT_RELOAD 1
#else
#    define U_HOT_RELOAD 0
#endif

#define UCLAMP(value, min, max) (value <= min) ? min : (value >= max) ? max : value;

#endif
