// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/exit.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int builtin_exit(const size_t argc, const char **argv) {
  if (argc > 2) {
    fprintf(stderr, "%s: too many arguments\n", argv[0]);
    return EXIT_FAILURE;
  }
  int exit_code;
  if (argc == 2) {
    long result = strtol(argv[1], (char **)NULL, 10);
    if (result == 0 && strcmp(argv[1], "0") != 0) {
      fprintf(stderr, "%s: expected numeric exit code instead of: '%s'\n",
              argv[0], argv[1]);
      return EXIT_FAILURE;
    }
    exit_code = (int)result;
  } else {
    exit_code = EXIT_SUCCESS;
  }
  exit(exit_code);
  return exit_code;
}
