// Copyright (c) 2026 Konrad Guzek

#include "src/lib/vector.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>

#define PUSH_BACK()                                                            \
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
  size_t size = size_t_vec_size(*vec_ptr);
  PUSH_BACK();
}

int push_back_string(struct string_vec **vec_ptr, char *value) {
  size_t size = string_vec_size(*vec_ptr);
  PUSH_BACK();
}

int push_back_string_unique(struct string_vec **vec_ptr, char *value) {
  size_t size = string_vec_size(*vec_ptr);
  for (size_t i = 0; i < size; i++) {
    if (strcmp((*vec_ptr)->value[i], value) == 0) {
      return EXIT_SUCCESS;
    }
  }
  PUSH_BACK();
}

int push_back_size_t_unique(struct size_t_vec **vec_ptr, size_t value) {
  size_t size = size_t_vec_size(*vec_ptr);
  for (size_t i = 0; i < size; i++) {
    if ((*vec_ptr)->value[i] == value) {
      return EXIT_SUCCESS;
    }
  }
  PUSH_BACK();
}

size_t size_t_vec_size(const struct size_t_vec *vec) {
  return vec == NULL ? 0 : vec->size;
}

size_t string_vec_size(const struct string_vec *vec) {
  if (vec != NULL) {}
  return vec == NULL ? 0 : vec->size;
}
