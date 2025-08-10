#include "core/event.h"
#include "core/umemory.h"
#include "containers/darray.h"
#include "defines.h"
#include <stdio.h>

typedef struct RegisteredEvent {
  void *listener;
  PFN_OnEvent callback;
} RegisteredEvent;

typedef struct EventCodeEntry {
  RegisteredEvent *events;
} EventCodeEntry;

#define MAX_MESSAGE_CODES 16384

typedef struct EventSystemState {
  EventCodeEntry registered[MAX_MESSAGE_CODES];
} EventSystemState;

static BOOLEAN isInitialized = FALSE;
static EventSystemState state;

BOOLEAN InitEvent() {
  if (isInitialized) return FALSE;
  isInitialized = FALSE;
  uzeromem(&state, sizeof(state));
  isInitialized = TRUE;
  S_TraceLogInfo("Event subsystem initialized successfuly");
  return TRUE;
}

void DestroyEvent() {
  for (u16 i = 0; i < MAX_MESSAGE_CODES; ++i) {
    if (state.registered[i].events != 0) {
      da_destroy(state.registered[i].events);
      state.registered[i].events = 0;
    }
  }
}

BOOLEAN RegisterEvent(u16 code, void *listener, PFN_OnEvent onEvent) {
  if (!isInitialized) return FALSE;
  if (state.registered[code].events == 0)
    state.registered[code].events = da_init(RegisteredEvent);

  u64 registeredCount = da_length(state.registered[code].events);
  for (u64 i = 0; i < registeredCount; ++i) {
    if (state.registered[code].events[i].listener == listener) {
      S_TraceLogWarn("(TODO) Event code listener already been registered, so can't listen it more then once");
      return FALSE;
    }
  }

  RegisteredEvent event;
  event.listener = listener;
  event.callback = onEvent;
  da_push(state.registered[code].events, event);

  return TRUE;
}

BOOLEAN UnregisterEvent(u16 code, void *listener, PFN_OnEvent onEvent) {
  if (!isInitialized) return FALSE;
  if (state.registered[code].events == 0)
    return FALSE; // TODO: warn
 
  u64 registeredCount = da_length(state.registered[code].events);
  for (u64 i = 0; i < registeredCount; ++i) {
    RegisteredEvent event = state.registered[code].events[i];
    if (event.listener == listener && event.callback == onEvent) {
      RegisteredEvent poppedEvent;
      da_pop_at(state.registered[code].events, i, &poppedEvent);
      return TRUE;
    }
  }

  // Events not found
  return FALSE;
}

BOOLEAN FireEvent(u16 code, void *sender, EventContext context) {
  if (!isInitialized) return FALSE;
  if (state.registered[code].events == 0)
    return FALSE;

  u64 registeredCount = da_length(state.registered[code].events);
  for (u64 i = 0; i < registeredCount; ++i) {
    RegisteredEvent event = state.registered[code].events[i];
    if (event.callback(code, sender, event.listener, context)) {
      return TRUE;
    }
  }

  return FALSE;
}
