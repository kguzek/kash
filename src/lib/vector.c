// Copyright (c) 2026 Konrad Guzek

#include "src/lib/vector.h"

#include <stddef.h>
#include <stdlib.h>

#define PUSH_BACK                                                              \
  size_t size = *vec == NULL ? 0 : (*vec)->size;                               \
  size_t new_size = size + 1;                                                  \
  if ((size & new_size) == 0) {                                                \
    size_t new_alloc_size = (size + new_size) * sizeof((*vec)->value[0]);      \
    void *temp = realloc(*vec, sizeof **vec + new_alloc_size);                 \
    if (temp == NULL) {                                                        \
      return EXIT_FAILURE;                                                     \
    }                                                                          \
    *vec = temp;                                                               \
  }                                                                            \
  (*vec)->value[size] = value;                                                 \
  (*vec)->size = new_size;                                                     \
  return EXIT_SUCCESS;

int push_back_size_t(struct size_t_vec **vec, size_t value) {
  PUSH_BACK;
}

int push_back_string(struct string_vec **vec, const char *value) {
  PUSH_BACK;
}
