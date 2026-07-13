// Copyright 2026 Konrad Guzek

#include "src/lib/strings.h"

#include <stdlib.h>
#include <string.h>

char *get_longest_common_prefix(char **strings, size_t nstrings) {
  size_t min_length = strlen(strings[0]);
  for (size_t i = 1; i < nstrings; i++) {
    size_t length = strlen(strings[i]);
    if (length < min_length) {
      min_length = length;
    }
  }
  char *prefix = malloc(min_length + 1);
  if (!prefix) {
    return NULL;
  }
  for (size_t i = 0; i < min_length; i++) {
    char c = strings[0][i];
    for (size_t j = 1; j < nstrings; j++) {
      if (strings[j][i] != c) {
        prefix[i] = '\0';
        return prefix;
      }
    }
    prefix[i] = c;
  }
  prefix[min_length] = '\0';
  return prefix;
}

int compare_strings(const void *lhs, const void *rhs) {
  return strcmp(*(const char **)lhs, *(const char **)rhs);
}
