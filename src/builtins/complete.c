// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/complete.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/vector.h"

static struct string_vec *registration_cmds;
static struct string_vec *registration_paths;

int builtin_complete(const size_t argc, const char **argv) {
  const char *program_name = argv[0];
  bool has_args = argc > 1;
  if (!has_args) {
    fprintf(stderr, "%s: missing option\n", program_name);
  }
  const char *option = argv[1];
  if (strcmp(option, "-p") == 0) {
    if (argc < 3) {
      fprintf(stderr, "%s: option %s requires at least one argument\n",
              program_name, option);
      return EXIT_FAILURE;
    }
    int current_exit_code, exit_code = EXIT_SUCCESS;
    for (size_t arg_idx = 2; arg_idx < argc; arg_idx++) {
      current_exit_code = print_registration_spec(program_name, argv[arg_idx]);
      if (current_exit_code != EXIT_SUCCESS) {
        exit_code = current_exit_code;
      }
    }
    return exit_code;
  }
  if (strcmp(option, "-C") == 0) {
    if (argc != 4) {
      fprintf(stderr, "%s: option %s requires exactly two arguments\n",
              program_name, option);
      return EXIT_FAILURE;
    }
    int exit_code =
        push_back_string(&registration_paths, argv[2]) != EXIT_SUCCESS;
    if (exit_code == EXIT_SUCCESS) {
      exit_code = push_back_string(&registration_cmds, argv[3]);
    }
    if (exit_code != EXIT_SUCCESS) {
      fprintf(stderr, "%s: registering completion specification failed\n",
              program_name);
    }
    return exit_code;
  }
  fprintf(stderr, "%s: invalid option: '%s'\n", program_name, option);
  return EXIT_FAILURE;
}

static int print_registration_spec(const char *program_name, const char *cmd) {
  bool found = false;
  if (registration_cmds != NULL && registration_paths != NULL) {
    for (size_t i = 0; i < registration_cmds->size; i++) {
      if (strcmp(registration_cmds->value[i], cmd) != 0) {
        continue;
      }
      printf("%s -C '%s' %s\n", program_name, registration_paths->value[i],
             cmd);
      found = true;
    }
  }
  if (found) {
    return EXIT_SUCCESS;
  }
  fprintf(stderr, "%s: %s: no completion specification\n", program_name, cmd);
  return EXIT_FAILURE;
}
