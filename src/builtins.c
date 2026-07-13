// Copyright 2026 Konrad Guzek

#include "src/builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/lib/path.h"

static const char *BUILTIN_COMMANDS[] = {"exit", "echo", "type", "pwd", "cd"};
static const int BUILTIN_COMMANDS_LENGTH =
    sizeof(BUILTIN_COMMANDS) / sizeof(BUILTIN_COMMANDS[0]);

int builtin_echo(const size_t argc, const char **argv) {
  if (argc > 1) {
    printf("%s", argv[1]);
    for (size_t i = 2; i < argc; i++) {
      printf(" %s", argv[i]);
    }
  }
  printf("\n");
  return EXIT_SUCCESS;
}

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

int builtin_pwd(const size_t argc, const char **argv) {
  char *cwd = getcwd(NULL, 0);
  if (cwd == NULL) {
    perror(argv[0]);
    return EXIT_FAILURE;
  }
  printf("%s\n", cwd);
  free(cwd);
  return EXIT_SUCCESS;
}

int builtin_cd(const size_t argc, const char **argv) {
  if (argc > 2) {
    fprintf(stderr, "%s: too many arguments\n", argv[0]);
    return EXIT_FAILURE;
  }
  const char *chdir_target;
  if (argc == 2) {
    chdir_target = argv[1];
  }
  if (argc < 2 || strcmp(chdir_target, "~") == 0) {
    chdir_target = getenv("HOME");
    if (chdir_target == NULL) {
      fprintf(stderr, "%s: failed to get home directory\n", argv[0]);
      return 2;
    }
  }
  if (chdir(chdir_target) != 0) {
    perror(argv[0]);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
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
