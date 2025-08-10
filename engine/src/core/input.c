#include "core/input.h"
#include "core/event.h"
#include "core/umemory.h"
#include "defines.h"
#include "stdio.h"

typedef struct KeyboardState {
  b8 keys[256];
} KeyboardState;

typedef struct MouseState {
  s16 x, y;
  u8 buttons[MOUSE_MAX_BUTTON];
} MouseState;

typedef struct InputState {
  KeyboardState kbCurrentState;
  KeyboardState kbPreviousState;
  MouseState mouseCurrentState;
  MouseState mousePreviousState;
} InputState;

static BOOLEAN initialized = FALSE;
static InputState state = {};

void InitInput() {
  uzeromem(&state, sizeof(InputState));
  initialized = TRUE;
  S_TraceLogInfo("Input subsystem initialized successfuly");
}

void DestroyInput() {
  initialized = FALSE;
}

void UpdateInput(u64 deltaTime) {
  if (!initialized) return;
  // Copy current state and copy to previous state
  umemcpy(&state.kbPreviousState, &state.kbCurrentState, sizeof(KeyboardState));
  umemcpy(&state.mousePreviousState, &state.mouseCurrentState, sizeof(MouseState));
}

void InputProcessKey(Keys key, b8 pressed) {
  if (state.kbCurrentState.keys[key] != pressed) {
    state.kbCurrentState.keys[key] = pressed;
    EventContext context;
    context.data.u16[0] = key;
    FireEvent(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, context);
  }
}

void InputProcessMouseButton(MouseButton button, b8 pressed) {
  if (state.mouseCurrentState.buttons[button] != pressed) {
    state.mouseCurrentState.buttons[button] = pressed;
    EventContext context;
    context.data.u16[0] = button;
    FireEvent(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED, 0, context);
  } 
}

void InputProcessMouseMove(s16 x, s16 y) {
  if (state.mouseCurrentState.x != x || state.mouseCurrentState.y != y) {
    state.mouseCurrentState.x = x;
    state.mouseCurrentState.y = y;
    EventContext context;
    context.data.u16[0] = x;
    context.data.u16[1] = y;
    FireEvent(EVENT_CODE_MOUSE_MOVE, 0, context);
  }
}

void InputProcessMouseWheel(s8 zDelta) {
  // No internal state update
  EventContext context;
  context.data.u8[0] = zDelta;
  FireEvent(EVENT_CODE_MOUSE_WHEEL, 0, context);
}

BOOLEAN InputIsKeyDown(Keys key) {
  if (!initialized) return FALSE;
  return state.kbCurrentState.keys[key] == TRUE;
}

BOOLEAN InputIsKeyUp(Keys key) {
  if (!initialized) return TRUE;
  return state.kbCurrentState.keys[key] == FALSE;
}

BOOLEAN InputWasKeyDown(Keys key) {
  if (!initialized) return FALSE;
  return state.kbPreviousState.keys[key] == TRUE;
}

BOOLEAN InputWasKeyUp(Keys key) {
  if (!initialized) return TRUE;
  return state.kbPreviousState.keys[key] == FALSE;
}

BOOLEAN InputIsMouseButtonDown(MouseButton button) {
  if (!initialized) return FALSE;
  return state.mouseCurrentState.buttons[button] == TRUE;
}

BOOLEAN InputIsMouseButtonUp(MouseButton button) {
  if (!initialized) return TRUE;
  return state.mouseCurrentState.buttons[button] == FALSE;
}

BOOLEAN InputWasMouseButtonDown(MouseButton button) {
  if (!initialized) return FALSE;
  return state.mousePreviousState.buttons[button] == TRUE;
}

BOOLEAN InputWasMouseButtonUp(MouseButton button) {
  if (!initialized) return TRUE;
  return state.mousePreviousState.buttons[button] == FALSE;
}

void InputGetMousePosition(i32 *x, i32 *y) {
  if (!initialized) {
    *x = 0;
    *y = 0;
    return;
  }
  *x = state.mousePreviousState.x;
  *y = state.mousePreviousState.y;
}
