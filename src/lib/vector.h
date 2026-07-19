// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_LIB_VECTOR_H_
#define SRC_LIB_VECTOR_H_

#include <stddef.h>
#include <stdlib.h>

struct size_t_vec {
  size_t size;
  size_t value[];
};

struct string_vec {
  size_t size;
  char *value[];
};

struct string_pair_vec {
  struct string_vec *keys;
  struct string_vec *values;
};

int push_back_size_t(struct size_t_vec **vec_ptr, size_t value);
int push_back_string(struct string_vec **vec_ptr, char *value);
int push_back_string_pair(struct string_pair_vec *vec_ptr, char *key,
                          char *value);
int push_back_size_t_unique(struct size_t_vec **vec_ptr, size_t value);
int push_back_string_unique(struct string_vec **vec_ptr, char *value);
size_t size_t_vec_size(const struct size_t_vec *vec);
size_t string_vec_size(const struct string_vec *vec);
size_t string_pair_vec_size(const struct string_pair_vec *vec);

char *string_pair_vec_key(const struct string_pair_vec *vec, size_t idx);
char *string_pair_vec_value(const struct string_pair_vec *vec, size_t idx);

int free_string_vec(struct string_vec *vec);
int free_string_pair_vec(struct string_pair_vec *vec);

#endif  // SRC_LIB_VECTOR_H_
