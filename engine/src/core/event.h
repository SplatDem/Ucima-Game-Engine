#pragma once

#include "defines.h"

typedef struct EventContext {
  union { // 128 bytes allocated on a stack for different kinds of events
    s64 i64[2]; // s64 = i64 = signed long long
    u64 u64[2];
    f64 f64[2];

    i32 i32[4];
    u32 u32[4];
    f32 f32[4];

    s16 i16[8]; // s16 = i16
    u16 u16[8];

    s8 i8[16]; // s8 = i8
    u8 u8[16];

    char c[16];
  } data;
} EventContext;

typedef b8 (*PFN_OnEvent)(u16 code, void *sender, void *listenerInst, EventContext data);

b8 InitEvent();
void DestroyEvent();

/**
 * @param code The event code to listen for.
 * @param A pointer to a listener instance. Nullable.
 * @param onEvent The Callback function pointer to be invoked when the event code is fired.
 * @returns TRUE if the event is successfuly registred, FALSE if it's not.
 */
UAPI b8 RegisterEvent(u16 code, void *listener, PFN_OnEvent onEvent);
UAPI b8 UnregisterEvent(u16 code, void *listener, PFN_OnEvent onEvent);
UAPI b8 FireEvent(u16 code, void *sender, EventContext context);

typedef enum SystemEventCode {
  EVENT_CODE_APPLICATION_QUIT = 0x01,
  EVENT_CODE_KEY_PRESSED = 0x02,
  EVENT_CODE_KEY_RELEASED = 0x03,
  EVENT_CODE_BUTTON_PRESSED = 0x04,
  EVENT_CODE_BUTTON_RELEASED = 0x05,
  EVENT_CODE_MOUSE_MOVE = 0x06,
  EVENT_CODE_MOUSE_WHEEL = 0x07,
  EVENT_CODE_WINDOW_RESIZED = 0x08,

  // Keep last!!!
  MAX_EVENT_CODE = 0xFF,
} SystemEventCode;
