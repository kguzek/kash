// Copyright 2026 Konrad Guzek

#include "src/lib/strings.h"

#include <stdlib.h>
#include <string.h>

#include "src/lib/vector.h"

char *get_longest_common_prefix(const struct string_vec *strings) {
  size_t strings_size = string_vec_size(strings);
  if (strings_size == 0) {
    return "";
  }
  size_t min_length = strlen(strings->value[0]);
  for (size_t i = 1; i < strings_size; i++) {
    size_t length = strlen(strings->value[i]);
    if (length < min_length) {
      min_length = length;
    }
  }
  char *prefix = malloc(min_length + 1);
  if (!prefix) {
    return NULL;
  }
  for (size_t i = 0; i < min_length; i++) {
    char c = strings->value[0][i];
    for (size_t j = 1; j < strings_size; j++) {
      if (strings->value[j][i] != c) {
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

bool is_zero(const char *str) {
  size_t i = 0;
  char c = str[i];
  bool seen_decimal = false;
  do {
    if (c != '0') {
      if (c == '.') {
        if (seen_decimal) {
          return false;
        }
        seen_decimal = true;
      } else {
        return false;
      }
    }
    c = str[++i];
  } while (c != '\0');
  return true;
}
