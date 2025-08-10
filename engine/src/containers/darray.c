#include "darray.h"
#include "core/umemory.h"
#include "defines.h"
#include <strings.h>
#include <stdio.h>

void *_da_init(u64 length, u64 stride) {
  u64 headerSize = DARRAY_FIELD_LENGTH * sizeof(u64);
  u64 arraySize = length * stride;
  u64 *newArray = ualloc(headerSize + arraySize, MEMORY_CATEGORY_DARRAY);
  umemset(newArray, 0, headerSize + arraySize);
  newArray[DARRAY_CAPACITY] = length;
  newArray[DARRAY_LENGTH] = 0;
  newArray[DARRAY_STRIDE] = stride;
  return (void*)(newArray + DARRAY_FIELD_LENGTH);
}

void _da_destroy(void *array) {
  u64 *header = (u64*)array - DARRAY_FIELD_LENGTH;
  u64 headerSize = DARRAY_FIELD_LENGTH * sizeof(u64);
  u64 totalSize = headerSize + header[DARRAY_CAPACITY]*header[DARRAY_STRIDE];
  ufree(header, totalSize, MEMORY_CATEGORY_DARRAY);
}

u64 _da_get_field(void *array, u64 field) {
  u64 *header = (u64*)array - DARRAY_FIELD_LENGTH;
  return header[field];
}

void _da_set_field(void *array, u64 field, u64 value) {
  u64 *header = (u64*)array - DARRAY_FIELD_LENGTH;
  header[field] = value;
}

void *_da_resize(void *array) {
  // Current array length and stride
  u64 length = da_length(array);
  u64 stride = da_stride(array);
  // Creating a new array almost same as an old array, but with capacity mult by DARRAY_RESIZE_FACTOR which is 2
  void *temp = _da_init(
    (DARRAY_RESIZE_FACTOR * da_capacity(array)),
    stride
  );
  // Copy old array to new
  umemcpy(temp, array, length);
  // Setting new array length
  _da_set_field(temp, DARRAY_LENGTH, length);
  // Destroing old array
  _da_destroy(array);
  // And return the new array
  return temp;
}

void *_da_push(void *array, const void *valuePtr) {
  u64 length = da_length(array);
  u64 stride = da_stride(array);
  if (length >= da_capacity(array))
    array = _da_resize(array);

  u64 addr = (u64)array;
  addr += (length * stride);
  umemcpy((void*)addr, valuePtr, stride);
  _da_set_field(array, DARRAY_LENGTH, length + 1);
  return array;
}

void _da_pop(void *array, void *dest) {
  u64 length = da_length(array);
  u64 stride = da_stride(array);
  u64 addr = (u64)array;
  addr += ((length - 1) * stride);
  umemcpy(dest, (void*)addr, stride);
  _da_set_field(array, DARRAY_LENGTH, length - 1);
}

void *_da_pop_at(void *array, u64 index, void *dest) {
  u64 length = da_length(array);
  u64 stride = da_stride(array);
  if (index >= length) {
    S_TraceLogWarn("Index out of bounds of array. Length: %llu, index: %llu", length, index);
    return array;
  }

  u64 addr = (u64)array;
  umemcpy(dest, (void*)(addr + (index * stride)), stride);
  if (index != length - 1) {
    umemcpy(
      (void*)(addr + (index * stride)),
      (void*)(addr + ((index + 1) * stride)),
      stride * (length - index)
    );
  }

  _da_set_field(array, DARRAY_LENGTH, length - 1);
  return array;
}

void *_da_insert(void *array, u64 index, void *valuePtr) {
    u64 length = da_length(array);
    u64 stride = da_stride(array);
    if (index >= length) {
        S_TraceLogWarn("Index out of bounds of array. Length: %llu, index: %llu", length, index);
        return array;
    }
    if (length >= da_capacity(array)) {
        array = _da_resize(array);
    }

    u64 addr = (u64)array;

    // Push element(s) from index forward out by one. This should
    // even happen if inserted at the last index.
    umemcpy(
        (void*)(addr + ((index + 1) * stride)),
        (void*)(addr + (index * stride)),
        stride * (length - index));

    // Set the value at the index
    umemcpy((void*)(addr + (index * stride)), valuePtr, stride);

    da_set_length(array, length + 1);
    return array;
}
