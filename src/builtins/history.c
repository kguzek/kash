// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/history.h"

#include <stdio.h>
#include <stdlib.h>

#include "src/lib/history.h"
#include "src/lib/vector.h"

int builtin_history(size_t argc, const char *argv[argc]) {
  struct string_vec *history = get_history_entries();
  size_t history_size = string_vec_size(history);
  size_t entry_number = 0;
  for (size_t i = 0; i < history_size; i++) {
    printf("    %lu  %s\n", ++entry_number, history->value[i]);
  }
  return EXIT_SUCCESS;
}
