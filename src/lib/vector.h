// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_LIB_VECTOR_H_
#define SRC_LIB_VECTOR_H_

#include <stddef.h>

struct vector_header {
  size_t size;
};

struct size_t_vec {
  size_t size;
  size_t value[];
};

struct string_vec {
  size_t size;
  const char *value[];
};

int push_back_size_t(struct size_t_vec **vec, size_t value);
int push_back_string(struct string_vec **vec, const char *value);

#endif  // SRC_LIB_VECTOR_H_
