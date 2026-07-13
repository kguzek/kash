// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/type.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/path.h"

static const char *BUILTIN_COMMANDS[] = {"exit", "echo", "type", "pwd", "cd"};
static const int BUILTIN_COMMANDS_LENGTH =
    sizeof(BUILTIN_COMMANDS) / sizeof(BUILTIN_COMMANDS[0]);

int builtin_type(const size_t argc, const char **argv) {
  if (argc < 2) {
    fprintf(stderr, ": not found\n");
    return 2;
  }
  int exit_code = EXIT_SUCCESS;
  for (size_t i = 1; i < argc; i++) {
    const char *input_command = argv[i];
    const int result = get_command_type(input_command);
    if (result != 0) {
      exit_code = result;
    }
  }
  return exit_code;
}

static int get_command_type(const char *input_command) {
  const char *command_match = NULL;
  for (int i = 0; i < BUILTIN_COMMANDS_LENGTH; i++) {
    const char *builtin_command = BUILTIN_COMMANDS[i];
    if (strcmp(builtin_command, input_command) == 0) {
      command_match = builtin_command;
      break;
    }
  }
  if (command_match != NULL) {
    printf("%s is a shell builtin\n", command_match);
    return EXIT_SUCCESS;
  }
  char *full_path = get_full_path(input_command);
  if (full_path != NULL) {
    printf("%s is %s\n", input_command, full_path);
    return EXIT_SUCCESS;
  }
  fprintf(stderr, "%s: not found\n", input_command);
  return EXIT_FAILURE;
}
