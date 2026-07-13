// Copyright 2026 Konrad Guzek

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <stddef.h>

int split_string(char *input, char *output[]);
int ring_bell();
char *get_longest_common_prefix(char **values, size_t nvalues);
int compare_strings(const void *lhs, const void *rhs);

#endif  // SRC_UTILS_H_
