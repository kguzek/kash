// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_LIB_VECTOR_H_
#define SRC_LIB_VECTOR_H_

#include <stddef.h>

struct size_t_vec {
  size_t size;
  size_t value[];
};

int push_back(struct size_t_vec **vec, size_t value);

#endif  // SRC_LIB_VECTOR_H_
