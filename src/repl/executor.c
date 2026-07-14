// Copyright (c) 2026 Konrad Guzek

#include "src/repl/executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "src/builtins/cd.h"
#include "src/builtins/echo.h"
#include "src/builtins/pwd.h"
#include "src/builtins/type.h"
#include "src/lib/config.h"
#include "src/lib/path.h"

int execute_commands(size_t cmdc, const char ***cmdv, const size_t argcv[]) {
  // for (size_t cmd_idx = 0; cmd_idx < cmdc; cmd_idx++) {
  //   size_t argc = argcv[cmd_idx];
  //   printf("command %lu (%lu arg%s):", cmd_idx, argc, argc == 1 ? "" : "s");
  //   for (size_t arg_idx = 0; arg_idx < argc; arg_idx++) {
  //     printf(" %s", cmdv[cmd_idx][arg_idx]);
  //   }
  //   printf("\n");
  // }

  const char **argv;
  size_t argc;
  int command_result = EXIT_SUCCESS;
  for (size_t cmd_idx = 0; cmd_idx < cmdc; cmd_idx++) {
    argc = argcv[cmd_idx];
    argv = cmdv[cmd_idx];
    command_result = execute_command(argc, argv);
    if (command_result == EXIT_FAILURE_EXHAUSTIVE) {
      break;
    }
  }
  return command_result;
}

static int execute_command(const size_t argc, const char **argv) {
  const char *first_word = argv[0];
  if (strcmp(first_word, "exit") == 0) {
    return EXIT_FAILURE_EXHAUSTIVE;
  }
  if (strcmp(first_word, "echo") == 0) {
    return builtin_echo(argc, argv);
  }
  if (strcmp(first_word, "type") == 0) {
    return builtin_type(argc, argv);
  }
  if (strcmp(first_word, "pwd") == 0) {
    return builtin_pwd(argc, argv);
  }
  if (strcmp(first_word, "cd") == 0) {
    return builtin_cd(argc, argv);
  }
  return try_run_external_program(argc, argv);
}

static int try_run_external_program(const size_t argc, const char **argv) {
  char *full_path = get_full_path(argv[0]);
  if (full_path == NULL) {
    fprintf(stderr, "%s: command not found\n", argv[0]);
    return EXIT_FAILURE;
  }
  return run_external_program(argc, argv, full_path);
}

static int run_external_program(const size_t argc, const char **argv,
                                const char *program_path) {
  int argv_copy_length = argc + 1;  // +1 for NULL terminator
  char **argv_copy = malloc(argv_copy_length * sizeof(*argv_copy));
  if (!argv_copy) {
    perror("malloc");
    return -1;
  }
  memcpy(argv_copy, argv, argc * sizeof(*argv_copy));
  argv_copy[argc] = NULL;

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return EXIT_FAILURE;
  }
  if (pid == 0) {
    execv(program_path, argv_copy);
  } else {
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
      return WEXITSTATUS(status);
    }
    return 2;
  }
  return EXIT_SUCCESS;
}
