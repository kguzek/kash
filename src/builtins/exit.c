// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/exit.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/config.h"
#include "src/lib/strings.h"

int builtin_exit(const size_t argc, const char **argv) {
  const char *command_name = argv[0];
  if (argc > 2) {
    fprintf(stderr, "%s: %s: too many arguments\n", PROGRAM_NAME, command_name);
    return EXIT_FAILURE;
  }
  int exit_code;
  if (argc == 2) {
    const char *option = argv[1];
    long result = strtol(option, (char **)NULL, 10);
    if (result == 0 && !is_zero(option)) {
      fprintf(stderr,
              "%s: %s: expected numeric exit code, instead received '%s'\n",
              PROGRAM_NAME, command_name, option);
      return EXIT_FAILURE;
    }
    exit_code = (int)result;
  } else {
    exit_code = EXIT_SUCCESS;
  }
  exit(exit_code);
  return exit_code;
}
