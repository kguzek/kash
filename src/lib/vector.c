// Copyright (c) 2026 Konrad Guzek

#include "src/lib/vector.h"

#include <stddef.h>
#include <stdlib.h>

#define VEC_SIZE(vec) vec == NULL ? 0 : vec->size;

#define PUSH_BACK                                                              \
  size_t size = VEC_SIZE((*vec_ptr));                                          \
  size_t new_size = size + 1;                                                  \
  if ((size & new_size) == 0) {                                                \
    size_t new_alloc_size = (size + new_size) * sizeof((*vec_ptr)->value[0]);  \
    void *temp = realloc(*vec_ptr, sizeof **vec_ptr + new_alloc_size);         \
    if (temp == NULL) {                                                        \
      return EXIT_FAILURE;                                                     \
    }                                                                          \
    *vec_ptr = temp;                                                           \
  }                                                                            \
  (*vec_ptr)->value[size] = value;                                             \
  (*vec_ptr)->size = new_size;                                                 \
  return EXIT_SUCCESS;

int push_back_size_t(struct size_t_vec **vec_ptr, size_t value) {
  PUSH_BACK;
}

int push_back_string(struct string_vec **vec_ptr, char *value) {
  PUSH_BACK;
}

size_t size_t_vec_size(const struct size_t_vec *vec) {
  return VEC_SIZE(vec);
}

size_t string_vec_size(const struct string_vec *vec) {
  return VEC_SIZE(vec);
}
