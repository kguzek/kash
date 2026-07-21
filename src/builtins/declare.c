// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/declare.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/config.h"

int builtin_declare(size_t argc, const char *argv[argc]) {
  const char *command_name = argv[0];
  if (argc == 1) {
    fprintf(stderr, "%s: %s: missing option\n", PROGRAM_NAME, command_name);
    return EXIT_FAILURE;
  }
  const char *option = argv[1];
  if (strcmp(option, "-p") == 0) {
    int result = EXIT_SUCCESS;
    for (size_t i = 2; i < argc; i++) {
      result = print_declared_variable(command_name, argv[i]);
    }
    return result;
  }
  fprintf(stderr, "%s: %s: invalid option '%s'\n", PROGRAM_NAME, command_name,
          option);
  return EXIT_FAILURE;
}

static int print_declared_variable(const char *command_name,
                                   const char *variable) {
  fprintf(stderr, "%s: %s: not found\n", command_name, variable);
  return EXIT_FAILURE;
}
