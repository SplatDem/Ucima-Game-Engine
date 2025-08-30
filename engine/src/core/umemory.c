// Memory subsystem tracs total memory usage
// and provide crossplatrform custom memory allocator

#include "umemory.h"
#include "ustring.h"
#include "platform/platform.h"
#include <defines.h>
#include <stdio.h>

// TODO: Change to custom string lib
#include <stdlib.h>
#include <string.h>

struct MemState {
  u64 totalAlloc;
  u64 categoriedAllocations[MEMORY_MAX_CATEGORIES];
};

static const char *memCategoryStr[MEMORY_MAX_CATEGORIES] = {
  "UNKNOWN", 
  "ARRAY", 
  "DARRAY", 
  "DICT", 
  "RING_QUEUE", 
  "BST", 
  "STRING", 
  "APPLICATION", 
  "JOB", 
  "TEXTURE", 
  "MAT_INST", 
  "RENDERER", 
  "GAME", 
  "TRANSFRORM", 
  "ENTITY",
  "ENTITY_NONE",
  "SCENE"
};

static struct MemState stats;

void InitMemory() {
  PlatformZeroMem(&stats, sizeof(stats));
  S_TraceLogInfo("Memory subsystem initialized successfuly");
}

void DestroyMemory() {
  RUNTIMEMESSAGE("TODO: DestroyMemory()");
}

void *ualloc(u64 size, MemoryCategory memCategory) {
  if (memCategory == MEMORY_CATEGORY_UNKNOWN)
    S_TraceLogWarn("Using MEMORY_CATEGORY_UNKNOWN. This allocator should be re-classed");

  // Tracing memory usage
  stats.totalAlloc += size;
  stats.categoriedAllocations[memCategory] += size;

  // TODO: Memory alignment
  void *block = PlatformAllocate(size, FALSE);
  PlatformZeroMem(block, size);

  return block;
}

void ufree(void *block, u64 size, MemoryCategory memCategory) {
  if (memCategory == MEMORY_CATEGORY_UNKNOWN)
    S_TraceLogWarn("Using MEMORY_CATEGORY_UNKNOWN. This allocator should be re-classed");

  stats.totalAlloc -= size;
  stats.categoriedAllocations[memCategory] -= size;

  // TODO: Memory alignment
  PlatformFree(block, FALSE);
}

void *uzeromem(void *block, u64 size) {
  return PlatformZeroMem(block, size);
}

void *umemcpy(void *dest, const void *src, u64 size) {
  return PlatformMemCopy(dest, src, size);
}

void *umemset(void *dest, i32 value, u64 size) {
  return PlatformMemSet(dest, value, size);
}

// In KB
char *DEBUG_GetMemUsageStr() {
  // If you didn't know, gb (for example) is a number mult by 1000
  // But computer uses 1024. It's gib
  const u64 gib = 1024*1024*1024;
  const u64 mib = 1024*1024;
  const u64 kib = 1024;

  char buffer[8000] = "Engine categorized memory usage:\n";
  u64 offset = strlen(buffer);

  for (u32 i = 0; i < MEMORY_MAX_CATEGORIES; ++i) {
    char unit[4] = "XiB"; // 4 for \0 which is null terminator
    float amount = 1.0f;
    if (stats.categoriedAllocations[i] >= gib) {
      unit[0] = 'G'; // Replacing XiB with GiB and so forth
      amount = stats.categoriedAllocations[i] / (float)gib;
    } else if (stats.categoriedAllocations[i] >= mib) {
      unit[0] = 'M';
      amount = stats.categoriedAllocations[i] / (float)mib;
    } else if (stats.categoriedAllocations[i] >= kib) {
      unit[0] = 'K';
      amount = stats.categoriedAllocations[i] / (float)kib;
    } else { // Bytes it's just bytes,
      unit[0] = 'B'; // so we are pushing B to X and 0 next as a null terminator
      unit[1] = 0;
      amount = (float)stats.categoriedAllocations[i];
    }
    i32 length = snprintf(buffer + offset, 8000, "\t%s: %.2f%s\n", memCategoryStr[i], amount, unit);
    offset += length;
  }
  char *string = ustrdup(buffer);
  return string;
}
