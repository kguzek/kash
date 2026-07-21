// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/history.h"

#include <stdio.h>
#include <stdlib.h>

#include "src/lib/config.h"
#include "src/lib/history.h"
#include "src/lib/vector.h"

int builtin_history(size_t argc, const char *argv[argc]) {
  struct string_vec *history = get_history_entries();
  size_t history_size = string_vec_size(history);
  size_t entry_start_idx = 0;
  if (argc > 1) {
    if (argc > 2) {
      fprintf(stderr, "%s: %s: expected at most one optional argument\n",
              PROGRAM_NAME, argv[0]);
    }
    long result = strtol(argv[1], (char **)NULL, 10);
    if (result < 0) {
      fprintf(stderr, "%s: %s: expected a nonnegative number\n", PROGRAM_NAME,
              argv[0]);
    }
    if (result >= 0 && result < history_size) {
      entry_start_idx = history_size - result;
    }
  }
  for (size_t i = entry_start_idx; i < history_size; i++) {
    printf("    %lu  %s\n", i + 1, history->value[i]);
  }
  return EXIT_SUCCESS;
}
