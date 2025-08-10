#include "core/ustring.h"
#include "core/umemory.h"

#include <string.h>

char *ustrdup(const char *str) {
  u64 len = strlen(str); // TODO: make string_len
  char *copy = ualloc(len + 1, MEMORY_CATEGORY_STRING);
  umemcpy(copy, str, len + 1);
  return copy;
}

BOOLEAN ustrcmp(const char *str0, const char *str1) {
  return strcmp(str0, str1) == 0;
}
