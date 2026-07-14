// Copyright (c) 2026 Konrad Guzek

#include "src/lib/vector.h"

#include <stddef.h>
#include <stdlib.h>

int push_back(struct size_t_vec **vec, size_t value) {
  size_t size = *vec == NULL ? 0 : (*vec)->size;
  size_t new_size = size + 1;
  // resize array if `new_size` is a power of 2; for example:
  // size     = 7    = 0b0111
  // new_size = 8    = 0b1000
  // size & new_size = 0b0000
  if ((size & new_size) == 0) {
    size_t new_alloc_size = (size + new_size) * sizeof((*vec)->value[0]);
    void *temp = realloc(*vec, sizeof **vec + new_alloc_size);
    if (temp == NULL) {
      return EXIT_FAILURE;
    }
    *vec = temp;
  }
  (*vec)->value[size] = value;
  (*vec)->size = new_size;
  return EXIT_SUCCESS;
}
