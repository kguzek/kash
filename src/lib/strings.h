// Copyright 2026 Konrad Guzek

#ifndef SRC_LIB_STRINGS_H_
#define SRC_LIB_STRINGS_H_

#include <stddef.h>

#include "src/lib/vector.h"

char *get_longest_common_prefix(const struct string_vec *strings);
int compare_strings(const void *lhs, const void *rhs);
bool is_zero(const char *str);

#endif  // SRC_LIB_STRINGS_H_
