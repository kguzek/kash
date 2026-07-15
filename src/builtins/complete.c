// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/complete.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int builtin_complete(const size_t argc, const char **argv) {
  bool print_spec = argc > 1 && strcmp(argv[1], "-p") == 0;
  if (print_spec) {
    if (argc < 3) {
      fprintf(stderr, "%s: option -p requires at least one argument\n",
              argv[0]);
      return EXIT_FAILURE;
    }

    for (size_t arg_idx = 2; arg_idx < argc; arg_idx++) {
      fprintf(stderr, "%s: %s: no completion specification\n", argv[0],
              argv[arg_idx]);
    }
    return EXIT_FAILURE;
  }
  fprintf(stderr, "%s: not implemented\n", argv[0]);
  return EXIT_FAILURE;
}
