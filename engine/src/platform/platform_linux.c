// Linux platform layer using X11 and XCB

#include "platform.h"

#if UPLATFORM_LINUX

#include "core/event.h"
#include "core/input.h"

#include <X11/X.h>
#include <bits/time.h>
#include <defines.h>
#include <xcb/xproto.h>
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <sys/time.h>

#define VK_USE_PLATFORM_XCB_KHR
#include <vulkan/vulkan.h>
#include "containers/darray.h"
#include "renderer/vulkan/vulkan_types.inl"

#if _POSIX_C_SOURCE >= 199309L
  #include <time.h>
#else
  #include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct InternalState {
  Display* display;
  xcb_connection_t* connection;
  xcb_window_t window;
  xcb_screen_t* screen;
  xcb_atom_t wmProtocols;
  xcb_atom_t wmDeleteWin;
  VkSurfaceKHR surface;
} InternalState;

Keys TranslateKeyCode(u32 xKeyCode);

static b8 SetupWindow(InternalState* state, const char* appName, i32 x, i32 y, i32 width, i32 height);
static b8 SetupWMProtocols(InternalState* state);
static void CleanupInternalState(InternalState* state);

b8 PlatformStartUp(
    PlatformState* platformState,
    const char* appName,
    i32 x, i32 y, i32 width, i32 height) {
  
  platformState->internalState = malloc(sizeof(InternalState));
  if (!platformState->internalState) {
    S_TraceLogError("Failed to allocate memory for platform state");
    return FALSE;
  }

  InternalState* state = (InternalState*)platformState->internalState;
  memset(state, 0, sizeof(InternalState));

  state->display = XOpenDisplay(NULL);
  if (!state->display) {
    S_TraceLogError("Failed to open X display");
    CleanupInternalState(state);
    return FALSE;
  }

  state->connection = XGetXCBConnection(state->display);
  if (xcb_connection_has_error(state->connection)) {
    S_TraceLogError("Failed to connect to X server via XCB");
    CleanupInternalState(state);
    return FALSE;
  }

  if (!SetupWindow(state, appName, x, y, width, height)) {
    S_TraceLogError("Failed to setup window");
    CleanupInternalState(state);
    return FALSE;
  }

  if (!SetupWMProtocols(state)) {
    S_TraceLogError("Failed to setup WM protocols");
    CleanupInternalState(state);
    return FALSE;
  }

  xcb_map_window(state->connection, state->window);

  if (xcb_flush(state->connection) <= 0) {
    S_TraceLogError("Failed to flush XCB connection");
    CleanupInternalState(state);
    return FALSE;
  }

  return TRUE;
}

void PlatformShutDown(PlatformState* platformState) {
  if (!platformState || !platformState->internalState) return;

  InternalState* state = (InternalState*)platformState->internalState;
  
  if (state->display)
    XAutoRepeatOn(state->display);

  if (state->connection && state->window)
    xcb_destroy_window(state->connection, state->window);

  if (state->display)
    XCloseDisplay(state->display);

  free(state);
  platformState->internalState = NULL;
}

b8 PlatformPollEvent(PlatformState  *platformState) {
  if (!platformState || !platformState->internalState) return TRUE;

  InternalState* state = (InternalState*)platformState->internalState;
  xcb_generic_event_t* event = NULL;
  b8 quitFlagged = FALSE;

  while ((event = xcb_poll_for_event(state->connection)) != NULL) {
    switch (event->response_type & ~0x80) {
      case XCB_KEY_PRESS: {
        xcb_key_press_event_t *kbEvent = (xcb_key_press_event_t*)event;
        b8 pressed = event->response_type = XCB_KEY_PRESS;
        xcb_keycode_t code = kbEvent->detail;
        KeySym keySym = XkbKeycodeToKeysym(
            state->display,
            (KeyCode)code,
            0,
            code & ShiftMask ? 1 : 0
        );
        Keys key = TranslateKeyCode(keySym);
        InputProcessKey(key, pressed);
      } break;

      case XCB_KEY_RELEASE: {
      } break;

      case XCB_BUTTON_PRESS: {
      } break;

      case XCB_BUTTON_RELEASE: {
        xcb_button_press_event_t *mouseEvent = (xcb_button_press_event_t*)event;
        b8 pressed = event->response_type == XCB_BUTTON_PRESS;
        MouseButton mouseButton = MOUSE_MAX_BUTTON;
        switch (mouseEvent->detail) {
          case XCB_BUTTON_INDEX_1:
            mouseButton = MOUSE_BUTTON_LEFT;
            break;
          case XCB_BUTTON_INDEX_2:
            mouseButton = MOUSE_BUTTON_MIDDLE;
            break;
          case XCB_BUTTON_INDEX_3:
            mouseButton = MOUSE_BUTTON_RIGHT;
            break;
        }
        if (mouseButton != MOUSE_MAX_BUTTON)
          InputProcessMouseButton(mouseButton, pressed);
      } break;

      case XCB_MOTION_NOTIFY: {
        xcb_motion_notify_event_t *moveEvent = (xcb_motion_notify_event_t*)event;
        InputProcessMouseMove(moveEvent->event_x, moveEvent->event_y);
      } break;

      case XCB_CONFIGURE_NOTIFY: {
        xcb_configure_notify_event_t *configureEvent = (xcb_configure_notify_event_t *)event;
        EventContext context;
        context.data.u16[0] = configureEvent->width;
        context.data.u16[1] = configureEvent->height;
        FireEvent(EVENT_CODE_WINDOW_RESIZED, 0, context);
      } break;

      case XCB_CLIENT_MESSAGE: {
        xcb_client_message_event_t* cm = (xcb_client_message_event_t*)event;
        if (cm->data.data32[0] == state->wmDeleteWin) {
          quitFlagged = TRUE;
        }
      } break;

      default:
        break;
    }

    free(event);
  }

  return !quitFlagged;
}

void* PlatformAllocate(u64 size, b8 aligned) {
  if (size == 0) return NULL;
  void* block = malloc(size);
  if (!block) {
    S_TraceLogError("Memory allocation failed for size: %llu", size);
  }
  return block;
}

void PlatformFree(void* block, b8 aligned) {
  if (block) {
    free(block);
  }
}

void *PlatformZeroMem(void *block, u64 size) {
  if (!block || size == 0) return block;
  return memset(block, 0, size);
}

void *PlatformMemCopy(void *dest, const void *src, u64 size) {
  if (!dest || !src || size == 0) return dest;
  return memcpy(dest, src, size);
}

void* PlatformMemSet(void* dest, i32 value, u64 size) {
  if (!dest || size == 0) return dest;
  return memset(dest, value, size);
}

void PlatformWriteConsole(const char* message, u8 color) {
  if (!message) return;
  const char* colorCodes[] = {"0;41", "1;31", "1;33", "1;34", "1;32", "1;30"};
  printf("\033[%sm%s\033[0m", colorCodes[color], message);
}

void PlatformWriteConsoleError(const char* message, u8 color) {
  if (!message) return;
  const char* colorCodes[] = {"0;41", "1;31", "1;33", "1;34", "1;32", "1;30"};
  fprintf(stderr, "\033[%sm%s\033[0m", colorCodes[color], message);
}

f64 PlatformGetClockTime() {
  struct timespec time;
  if (clock_gettime(CLOCK_MONOTONIC, &time) != 0) {
    S_TraceLogError("Failed to get clock time");
    return 0.0;
  }
  return (f64)time.tv_sec + (f64)time.tv_nsec * 1e-9;
}

void PlatformSleep(u64 msecs) {
#if _POSIX_C_SOURCE >= 199309L
  struct timespec ts;
  ts.tv_sec = msecs / 1000;
  ts.tv_nsec = (msecs % 1000) * 1000000;
  nanosleep(&ts, NULL);
#else
  if (msecs >= 1000) {
    sleep(msecs / 1000);
  }
  usleep((msecs % 1000) * 1000);
#endif
}

static b8 SetupWindow(InternalState* state, const char* appName, i32 x, i32 y, i32 width, i32 height) {
  const xcb_setup_t* setup = xcb_get_setup(state->connection);
  xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
  state->screen = it.data;

  if (!state->screen) {
    S_TraceLogError("Failed to get XCB screen");
    return FALSE;
  }

  state->window = xcb_generate_id(state->connection);
  u32 eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  u32 eventValue = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                  XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                  XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                  XCB_EVENT_MASK_STRUCTURE_NOTIFY;
  u32 valueList[] = {state->screen->black_pixel, eventValue};

  xcb_void_cookie_t cookie = xcb_create_window(
      state->connection,
      XCB_COPY_FROM_PARENT,
      state->window,
      state->screen->root,
      x, y, width, height, 0,
      XCB_WINDOW_CLASS_INPUT_OUTPUT,
      state->screen->root_visual,
      eventMask, valueList);

  xcb_generic_error_t* error = xcb_request_check(state->connection, cookie);
  if (error) {
    S_TraceLogError("Failed to create window: %d", error->error_code);
    free(error);
    return FALSE;
  }

  xcb_change_property(
      state->connection,
      XCB_PROP_MODE_REPLACE,
      state->window,
      XCB_ATOM_WM_NAME,
      XCB_ATOM_STRING,
      8,
      strlen(appName),
      appName);

  return TRUE;
}

void PlatformGetRequiredExtensionNames(const char ***names) {
  da_push(*names, &"VK_KHR_xcb_surface");
}

BOOLEAN PlatformCreateVulkanSurface(struct PlatformState *pState, VulkanContext *context) {
  InternalState *state = (InternalState *)pState->internalState;

  VkXcbSurfaceCreateInfoKHR createInfo = {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
  createInfo.connection = state->connection;
  createInfo.window = state->window;

  VkResult result = vkCreateXcbSurfaceKHR(
    context->instance,
    &createInfo,
    context->allocator,
    &state->surface
  );

  if (result != VK_SUCCESS) {
    S_TraceLogError("Failed to create vulkan surface");
    return FALSE;
  }

  context->surface = state->surface;
  return TRUE;
}

static b8 SetupWMProtocols(InternalState* state) {
  xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(
      state->connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
  xcb_intern_atom_cookie_t wmProtocolsCookie = xcb_intern_atom(
      state->connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");

  xcb_intern_atom_reply_t* wmDeleteReply = xcb_intern_atom_reply(
      state->connection, wmDeleteCookie, NULL);
  xcb_intern_atom_reply_t* wmProtocolsReply = xcb_intern_atom_reply(
      state->connection, wmProtocolsCookie, NULL);

  if (!wmDeleteReply || !wmProtocolsReply) {
    S_TraceLogError("Failed to get WM protocols");
    if (wmDeleteReply) free(wmDeleteReply);
    if (wmProtocolsReply) free(wmProtocolsReply);
    return FALSE;
  }

  state->wmDeleteWin = wmDeleteReply->atom;
  state->wmProtocols = wmProtocolsReply->atom;

  free(wmDeleteReply);
  free(wmProtocolsReply);

  xcb_change_property(
      state->connection,
      XCB_PROP_MODE_REPLACE,
      state->window,
      state->wmProtocols,
      4, 32, 1,
      &state->wmDeleteWin);

  return TRUE;
}

static void CleanupInternalState(InternalState* state) {
  if (!state) return;

  if (state->display) {
    XCloseDisplay(state->display);
    state->display = NULL;
  }

  free(state);
}

Keys TranslateKeyCode(u32 xKeyCode) {
    switch (xKeyCode) {
    case XK_BackSpace:
        return KEY_BACKSPACE;
    case XK_Return:
        return KEY_ENTER;
    case XK_Tab:
        return KEY_TAB;
        // case XK_Shift: return KEY_SHIFT;
        // case XK_Control: return KEY_CONTROL;

    case XK_Pause:
        return KEY_PAUSE;
    case XK_Caps_Lock:
        return KEY_CAPITAL;

    case XK_Escape:
        return KEY_ESCAPE;

        // Not supported
        // case : return KEY_CONVERT;
        // case : return KEY_NONCONVERT;
        // case : return KEY_ACCEPT;

    case XK_Mode_switch:
        return KEY_MODECHANGE;

    case XK_space:
        return KEY_SPACE;
    case XK_Prior:
        return KEY_PAGEUP;
    case XK_Next:
        return KEY_PAGEDOWN;
    case XK_End:
        return KEY_END;
    case XK_Home:
        return KEY_HOME;
    case XK_Left:
        return KEY_LEFT;
    case XK_Up:
        return KEY_UP;
    case XK_Right:
        return KEY_RIGHT;
    case XK_Down:
        return KEY_DOWN;
    case XK_Select:
        return KEY_SELECT;
    case XK_Print:
        return KEY_PRINT;
    case XK_Execute:
        return KEY_EXECUTE;
    // case XK_snapshot: return KEY_SNAPSHOT; // not supported
    case XK_Insert:
        return KEY_INSERT;
    case XK_Delete:
        return KEY_DELETE;
    case XK_Help:
        return KEY_HELP;

    case XK_Meta_L:
    case XK_Super_L:
        // Treat the "meta" key (if mapped) as super
        return KEY_LSUPER;
    case XK_Meta_R:
    case XK_Super_R:
        // Treat the "meta" key (if mapped) as super
        return KEY_RSUPER;
        // case XK_apps: return KEY_APPS; // not supported

        // case XK_sleep: return KEY_SLEEP; //not supported

    case XK_KP_0:
        return KEY_NUMPAD0;
    case XK_KP_1:
        return KEY_NUMPAD1;
    case XK_KP_2:
        return KEY_NUMPAD2;
    case XK_KP_3:
        return KEY_NUMPAD3;
    case XK_KP_4:
        return KEY_NUMPAD4;
    case XK_KP_5:
        return KEY_NUMPAD5;
    case XK_KP_6:
        return KEY_NUMPAD6;
    case XK_KP_7:
        return KEY_NUMPAD7;
    case XK_KP_8:
        return KEY_NUMPAD8;
    case XK_KP_9:
        return KEY_NUMPAD9;
    case XK_multiply:
        return KEY_MULTIPLY;
    case XK_KP_Add:
        return KEY_ADD;
    case XK_KP_Separator:
        return KEY_SEPARATOR;
    case XK_KP_Subtract:
        return KEY_SUBTRACT;
    case XK_KP_Decimal:
        return KEY_DECIMAL;
    case XK_KP_Divide:
        return KEY_DIVIDE;
    case XK_F1:
        return KEY_F1;
    case XK_F2:
        return KEY_F2;
    case XK_F3:
        return KEY_F3;
    case XK_F4:
        return KEY_F4;
    case XK_F5:
        return KEY_F5;
    case XK_F6:
        return KEY_F6;
    case XK_F7:
        return KEY_F7;
    case XK_F8:
        return KEY_F8;
    case XK_F9:
        return KEY_F9;
    case XK_F10:
        return KEY_F10;
    case XK_F11:
        return KEY_F11;
    case XK_F12:
        return KEY_F12;
    case XK_F13:
        return KEY_F13;
    case XK_F14:
        return KEY_F14;
    case XK_F15:
        return KEY_F15;
    case XK_F16:
        return KEY_F16;
    case XK_F17:
        return KEY_F17;
    case XK_F18:
        return KEY_F18;
    case XK_F19:
        return KEY_F19;
    case XK_F20:
        return KEY_F20;
    case XK_F21:
        return KEY_F21;
    case XK_F22:
        return KEY_F22;
    case XK_F23:
        return KEY_F23;
    case XK_F24:
        return KEY_F24;

    case XK_Num_Lock:
        return KEY_NUMLOCK;
    case XK_Scroll_Lock:
        return KEY_SCROLL;

    case XK_KP_Equal:
        return KEY_NUMPAD_EQUAL;

    case XK_Shift_L:
        return KEY_LSHIFT;
    case XK_Shift_R:
        return KEY_RSHIFT;
    case XK_Control_L:
        return KEY_LCONTROL;
    case XK_Control_R:
        return KEY_RCONTROL;
    case XK_Alt_L:
        return KEY_LALT;
    case XK_Alt_R:
        return KEY_RALT;

    case XK_semicolon:
        return KEY_SEMICOLON;
    case XK_plus:
        return KEY_EQUAL;
    case XK_comma:
        return KEY_COMMA;
    case XK_minus:
        return KEY_MINUS;
    case XK_period:
        return KEY_PERIOD;
    case XK_slash:
        return KEY_SLASH;
    case XK_grave:
        return KEY_GRAVE;

    case XK_0:
        return KEY_0;
    case XK_1:
        return KEY_1;
    case XK_2:
        return KEY_2;
    case XK_3:
        return KEY_3;
    case XK_4:
        return KEY_4;
    case XK_5:
        return KEY_5;
    case XK_6:
        return KEY_6;
    case XK_7:
        return KEY_7;
    case XK_8:
        return KEY_8;
    case XK_9:
        return KEY_9;

    case XK_a:
    case XK_A:
        return KEY_A;
    case XK_b:
    case XK_B:
        return KEY_B;
    case XK_c:
    case XK_C:
        return KEY_C;
    case XK_d:
    case XK_D:
        return KEY_D;
    case XK_e:
    case XK_E:
        return KEY_E;
    case XK_f:
    case XK_F:
        return KEY_F;
    case XK_g:
    case XK_G:
        return KEY_G;
    case XK_h:
    case XK_H:
        return KEY_H;
    case XK_i:
    case XK_I:
        return KEY_I;
    case XK_j:
    case XK_J:
        return KEY_J;
    case XK_k:
    case XK_K:
        return KEY_K;
    case XK_l:
    case XK_L:
        return KEY_L;
    case XK_m:
    case XK_M:
        return KEY_M;
    case XK_n:
    case XK_N:
        return KEY_N;
    case XK_o:
    case XK_O:
        return KEY_O;
    case XK_p:
    case XK_P:
        return KEY_P;
    case XK_q:
    case XK_Q:
        return KEY_Q;
    case XK_r:
    case XK_R:
        return KEY_R;
    case XK_s:
    case XK_S:
        return KEY_S;
    case XK_t:
    case XK_T:
        return KEY_T;
    case XK_u:
    case XK_U:
        return KEY_U;
    case XK_v:
    case XK_V:
        return KEY_V;
    case XK_w:
    case XK_W:
        return KEY_W;
    case XK_x:
    case XK_X:
        return KEY_X;
    case XK_y:
    case XK_Y:
        return KEY_Y;
    case XK_z:
    case XK_Z:
        return KEY_Z;

    default:
        return 0;
    }
}

#endif
