// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_LIB_VECTOR_H_
#define SRC_LIB_VECTOR_H_

#include <stddef.h>

struct size_t_vec {
  size_t size;
  size_t value[];
};

struct string_vec {
  size_t size;
  char *value[];
};

int push_back_size_t(struct size_t_vec **vec_ptr, size_t value);
int push_back_string(struct string_vec **vec_ptr, char *value);
size_t size_t_vec_size(const struct size_t_vec *vec);
size_t string_vec_size(const struct string_vec *vec);

#endif  // SRC_LIB_VECTOR_H_
