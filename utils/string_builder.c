#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *buffer;
  size_t capacity;
  size_t offset;
} SB_StringArena;

static void sb_init(SB_StringArena *arena, size_t initial_capacity) {
  arena->buffer = malloc(initial_capacity);
  arena->capacity = initial_capacity;
  arena->offset = 0;
}

static void sb_append(SB_StringArena *arena, const char *str) {
  size_t len = strlen(str);
  size_t needed = arena->offset + len + 1;
  if (needed > arena->capacity) {
    size_t new_cap = arena->capacity * 2;
    while (new_cap < needed)
      new_cap *= 2;
    arena->buffer = realloc(arena->buffer, new_cap);
    arena->capacity = new_cap;
  }
  memcpy(arena->buffer + arena->offset, str, len + 1);
  arena->offset += len;
}

static void sb_clear(SB_StringArena *arena) {
  arena->offset = 0;
  if (arena->buffer)
    arena->buffer[0] = '\0';
}

static void sb_free(SB_StringArena *arena) {
  free(arena->buffer);
  arena->buffer = NULL;
  arena->capacity = 0;
  arena->offset = 0;
}

static const char *sb_get(SB_StringArena *arena) { return arena->buffer; }

static size_t sb_length(SB_StringArena *arena) { return arena->offset; }

static void sb_truncate(SB_StringArena *arena, size_t new_offset) {
  if (new_offset < arena->offset) {
    arena->offset = new_offset;
    arena->buffer[new_offset] = '\0';
  }
}
