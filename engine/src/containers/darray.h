#pragma once

#include "defines.h"

enum {
  DARRAY_CAPACITY,
  DARRAY_LENGTH,
  DARRAY_STRIDE,
  DARRAY_FIELD_LENGTH,
};

UAPI void *_da_init(u64 length, u64 stride);
UAPI void _da_destroy(void *array);
UAPI u64 _da_get_field(void *array, u64 field);
UAPI void _da_set_field(void *array, u64 field, u64 value);
UAPI void *_da_resize(void *array);
UAPI void *_da_push(void *array, const void *valuePtr);
UAPI void _da_pop(void *array, void *dest);
UAPI void *_da_pop_at(void *array, u64 index, void *dest);
UAPI void *_da_insert(void *array, u64 index, void *valuePtr);

#define DARRAY_DEFAULT_CAPACITY 1
#define DARRAY_RESIZE_FACTOR 2

#define da_init(type) \
  _da_init(DARRAY_DEFAULT_CAPACITY, sizeof(type))

#define da_reserve(type, capacity) \
  _da_init(capacity, sizeof(type))

#define da_destroy(array) _da_destroy(array);

#define da_push(array, value) \
  { \
    typeof(value) temp = value; \
    array = _da_push(array, &temp); \
  }

#define da_pop(array, valuePtr) \
  _da_pop(array, valuePtr)

#define da_insert(array, index, valuePtr) \
  { \
    typeof(value) temp = value; \
    array = _da_insert(array, index, &temp); \
  }

#define da_pop_at(array, index, valuePtr) \
  _da_pop_at(array, index, valuePtr)

#define da_clear(array) \
  _da_set_field(array, DARRAY_LENGTH, 0)

#define da_capacity(array) \
  _da_get_field(array, DARRAY_CAPACITY)

#define da_length(array) \
  _da_get_field(array, DARRAY_LENGTH)

#define da_stride(array) \
  _da_get_field(array, DARRAY_STRIDE)

#define da_set_length(array, value) \
  _da_set_field(array, DARRAY_LENGTH, value)
