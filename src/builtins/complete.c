// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/complete.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/config.h"
#include "src/lib/vector.h"
#include "src/repl/completions.h"

int builtin_complete(const size_t argc, const char **argv) {
  const char *command_name = argv[0];
  bool has_args = argc > 1;
  if (!has_args) {
    fprintf(stderr, "%s: %s: missing option\n", PROGRAM_NAME, command_name);
    return EXIT_FAILURE;
  }
  const char *option = argv[1];
  if (strcmp(option, "-p") == 0) {
    if (argc < 3) {
      fprintf(stderr, "%s: %s: option %s requires at least one argument\n",
              PROGRAM_NAME, command_name, option);
      return EXIT_FAILURE;
    }
    int current_exit_code, exit_code = EXIT_SUCCESS;
    for (size_t arg_idx = 2; arg_idx < argc; arg_idx++) {
      current_exit_code =
          print_registratered_spec_paths(command_name, argv[arg_idx]);
      if (current_exit_code != EXIT_SUCCESS) {
        exit_code = current_exit_code;
      }
    }
    return exit_code;
  }
  if (strcmp(option, "-C") == 0) {
    if (argc != 4) {
      fprintf(stderr, "%s: %s: option %s requires exactly two arguments\n",
              PROGRAM_NAME, command_name, option);
      return EXIT_FAILURE;
    }
    int exit_code = register_completion_spec(argv[3], argv[2]);
    if (exit_code != EXIT_SUCCESS) {
      fprintf(stderr, "%s: %s: registering completion specification failed\n",
              PROGRAM_NAME, command_name);
    }
    return exit_code;
  }
  if (strcmp(option, "-r") == 0) {
    if (argc != 3) {
      fprintf(stderr, "%s: %s: option %s requires exactly one argument\n",
              PROGRAM_NAME, command_name, option);
      return EXIT_FAILURE;
    }

    int exit_code = unregister_completion_spec(argv[2]);
    if (exit_code != EXIT_SUCCESS) {
      fprintf(stderr, "%s: %s: unregistering completion specification failed\n",
              PROGRAM_NAME, command_name);
    }
    return exit_code;
  }
  fprintf(stderr, "%s: %s: invalid option: '%s'\n", PROGRAM_NAME, command_name,
          option);
  return EXIT_FAILURE;
}

static int print_registratered_spec_paths(const char *command_name,
                                          const char *cmd) {
  struct string_vec *spec_paths = NULL;
  size_t spec_paths_size =
      populate_registered_completion_specs(cmd, &spec_paths);
  if (spec_paths_size == 0) {
    fprintf(stderr, "%s: %s: %s: no completion specification\n", PROGRAM_NAME,
            command_name, cmd);
    return EXIT_FAILURE;
  }
  for (size_t i = 0; i < spec_paths_size; i++) {
    printf("%s -C '%s' %s\n", command_name, spec_paths->value[i], cmd);
  }
  return EXIT_SUCCESS;
}
