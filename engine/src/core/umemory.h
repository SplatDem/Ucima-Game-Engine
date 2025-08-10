#pragma once

#include "defines.h"

typedef enum MemoryCategory {
  MEMORY_CATEGORY_UNKNOWN,
  MEMORY_CATEGORY_ARRAY,
  MEMORY_CATEGORY_DARRAY,
  MEMORY_CATEGORY_DICT,
  MEMORY_CATEGORY_RING_QUEUE,
  MEMORY_CATEGORY_RING_BST,
  MEMORY_CATEGORY_STRING,
  MEMORY_CATEGORY_APPLICATION,
  MEMORY_CATEGORY_JOB,
  MEMORY_CATEGORY_TEXTURE,
  MEMORY_CATEGORY_MAT_INST,
  MEMORY_CATEGORY_RENDERER,
  MEMORY_CATEGORY_GAME,
  MEMORY_CATEGORY_TRANSFORM,
  MEMORY_CATEGORY_ENTITY,
  MEMORY_CATEGORY_ENTITY_NONE,
  MEMORY_CATEGORY_SCENE,

  // Keep last!!!
  MEMORY_MAX_CATEGORIES,
} MemoryCategory;

UAPI void InitMemory();
UAPI void DestroyMemory();

UAPI void *ualloc(u64 size, MemoryCategory memCategory);
UAPI void ufree(void *block, u64 size, MemoryCategory memCategory);
UAPI void *uzeromem(void *block, u64 size);
UAPI void *umemcpy(void *dest, const void *src, u64 size);
UAPI void *umemset(void *dest, i32 value, u64 size);
UAPI char *DEBUG_GetMemUsageStr();
