// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_LIB_VECTOR_H_
#define SRC_LIB_VECTOR_H_

#include <stddef.h>
#include <stdlib.h>

#include "src/lib/jobs.h"

#define VECTOR_DECLARE(name, type)                                             \
  struct name##_vec {                                                          \
    size_t size;                                                               \
    type value[];                                                              \
  };                                                                           \
                                                                               \
  size_t name##_vec_size(const struct name##_vec *vec);                        \
  int push_back_##name(struct name##_vec **vec_ptr, type value);               \
  int push_back_##name##_unique(struct name##_vec **vec_ptr, type value);      \
  static bool name##_equal(const type a, const type b);

#define PTR_VECTOR_DECLARE(name, type)                                         \
  VECTOR_DECLARE(name, type);                                                  \
  int free_##name##_vec(struct name##_vec *vec);

VECTOR_DECLARE(size_t, size_t);
PTR_VECTOR_DECLARE(string, char *);
PTR_VECTOR_DECLARE(job, struct job_definition *);

struct string_pair_vec {
  struct string_vec *keys;
  struct string_vec *values;
};

size_t string_pair_vec_size(const struct string_pair_vec *vec);
int push_back_string_pair(struct string_pair_vec *vec_ptr, char *key,
                          char *value);
int free_string_pair_vec(struct string_pair_vec *vec);
char *string_pair_vec_key(const struct string_pair_vec *vec, size_t idx);
char *string_pair_vec_value(const struct string_pair_vec *vec, size_t idx);

#endif  // SRC_LIB_VECTOR_H_
