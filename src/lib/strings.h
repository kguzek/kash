// Copyright 2026 Konrad Guzek

#ifndef SRC_LIB_STRINGS_H_
#define SRC_LIB_STRINGS_H_

#include <stddef.h>

char *get_longest_common_prefix(char **values, size_t nvalues);
int compare_strings(const void *lhs, const void *rhs);

#endif  // SRC_LIB_STRINGS_H_
