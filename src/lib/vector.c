// Copyright (c) 2026 Konrad Guzek

#include "src/lib/vector.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>

#include "src/lib/jobs.h"

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

#define VECTOR_IMPL(name, type)                                                \
  size_t name##_vec_size(const struct name##_vec *vec) {                       \
    return vec == NULL ? 0 : vec->size;                                        \
  }                                                                            \
                                                                               \
  int push_back_##name(struct name##_vec **vec_ptr, type value) {              \
    size_t size = name##_vec_size(*vec_ptr);                                   \
    PUSH_BACK();                                                               \
  }                                                                            \
                                                                               \
  int push_back_##name##_unique(struct name##_vec **vec_ptr, type value) {     \
    size_t size = name##_vec_size(*vec_ptr);                                   \
    for (size_t i = 0; i < size; i++) {                                        \
      if (name##_equal((*vec_ptr)->value[i], value)) {                         \
        return EXIT_SUCCESS;                                                   \
      }                                                                        \
    }                                                                          \
    PUSH_BACK();                                                               \
  }

#define PTR_VECTOR_IMPL(name, type)                                            \
  int free_##name##_vec(struct name##_vec *vec) {                              \
    if (vec == NULL) {                                                         \
      return EXIT_SUCCESS;                                                     \
    }                                                                          \
    for (size_t i = 0; i < vec->size; i++) {                                   \
      free(vec->value[i]);                                                     \
    }                                                                          \
    return EXIT_SUCCESS;                                                       \
  }                                                                            \
  VECTOR_IMPL(name, type);

VECTOR_IMPL(size_t, size_t);
PTR_VECTOR_IMPL(string, char *);
PTR_VECTOR_IMPL(job, struct job_definition *);
PTR_VECTOR_IMPL(variable, struct variable_definition *);

bool string_equal(const char *a, const char *b) {
  return strcmp(a, b) == 0;
}

bool size_t_equal(const size_t a, const size_t b) {
  return a == b;
}

bool job_equal(const struct job_definition *a, const struct job_definition *b) {
  return a->id == b->id || a->pid == b->pid;
}

bool variable_equal(const struct variable_definition *a,
                    const struct variable_definition *b) {
  return a->type == b->type && strcmp(a->name, b->name) == 0;
}

size_t string_pair_vec_size(const struct string_pair_vec *vec) {
  size_t keys_size = string_vec_size(vec->keys);
  size_t values_size = string_vec_size(vec->values);
  if (keys_size != values_size) {
    fprintf(
        stderr,
        "string_pair_vec_size: inconsistent sizes: %lu key(s) and %lu value(s)",
        keys_size, values_size);
    return 0;
  }
  return keys_size;
}

int push_back_string_pair(struct string_pair_vec *vec_ptr, char *key,
                          char *value) {
  int result = push_back_string(&vec_ptr->keys, key);
  if (result == EXIT_SUCCESS) {
    result = push_back_string(&vec_ptr->values, value);
  }
  return result;
}

int free_string_pair_vec(struct string_pair_vec *vec) {
  if (vec == NULL) {
    return EXIT_SUCCESS;
  }
  size_t size = string_pair_vec_size(vec);
  for (size_t i = 0; i < size; i++) {
    free(string_pair_vec_key(vec, i));
    free(string_pair_vec_value(vec, i));
  }
  return EXIT_SUCCESS;
}

char *string_pair_vec_key(const struct string_pair_vec *vec, size_t idx) {
  return vec->keys->value[idx];
}

char *string_pair_vec_value(const struct string_pair_vec *vec, size_t idx) {
  return vec->values->value[idx];
}
