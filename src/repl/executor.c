// Copyright (c) 2026 Konrad Guzek

#include "src/repl/executor.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "src/builtins/cd.h"
#include "src/builtins/complete.h"
#include "src/builtins/echo.h"
#include "src/builtins/exit.h"
#include "src/builtins/pwd.h"
#include "src/builtins/type.h"
#include "src/lib/config.h"
#include "src/lib/path.h"

#define BUILTIN_PID_VALUE 0

int execute_commands(size_t cmdc, const char ***cmdv, const bool *cmd_pipes,
                     const size_t argcv[]) {
  if (cmd_pipes[cmdc - 1]) {
    // sanity check; parser ensures this
    fprintf(stderr, "%s: |: final pipeline command has no target\n",
            PROGRAM_NAME);
    return EXIT_FAILURE;
  }

  int pipes[cmdc][2];
  for (size_t cmd_idx = 0; cmd_idx < cmdc; cmd_idx++) {
    if (!cmd_pipes[cmd_idx]) {
      continue;
    }
    if (pipe(pipes[cmd_idx]) == 0) {
      continue;
    }
    perror("pipe");
    for (size_t cmd_jdx = 0; cmd_jdx < cmd_idx; cmd_jdx++) {
      close(pipes[cmd_jdx][0]);
      close(pipes[cmd_jdx][1]);
    }
    return EXIT_FAILURE;
  }

  size_t argc;
  const char **argv;
  pid_t pids[cmdc];
  int saved_stdin, saved_stdout, command_result;
  // TODO(kguzek): this could be space-optimized, as we
  // only need to return the final command's exit code
  int exit_codes[cmdc];
  bool has_input_pipe = false, has_output_pipe = false;
  for (size_t cmd_idx = 0; cmd_idx < cmdc; cmd_idx++) {
    argc = argcv[cmd_idx];
    argv = cmdv[cmd_idx];
    has_input_pipe = has_output_pipe;
    has_output_pipe = cmd_pipes[cmd_idx];
    saved_stdin = dup(STDIN_FILENO);
    saved_stdout = dup(STDOUT_FILENO);
    if (has_input_pipe) {
      // attach previous pipe to the program's stdin
      dup2(pipes[cmd_idx - 1][0], STDIN_FILENO);
    }
    if (has_output_pipe) {
      // attach pipe to the program's stdout
      dup2(pipes[cmd_idx][1], STDOUT_FILENO);
    }
    command_result = execute_command(argc, argv, &pids[cmd_idx]);
    if (has_input_pipe) {
      close(pipes[cmd_idx - 1][0]);
      dup2(saved_stdin, STDIN_FILENO);
    }
    if (has_output_pipe) {
      close(pipes[cmd_idx][1]);
      dup2(saved_stdout, STDOUT_FILENO);
    }
    if (pids[cmd_idx] == BUILTIN_PID_VALUE) {
      // was not fork+exec'd (e.g. builtin)
      exit_codes[cmd_idx] = command_result;
    }
  }
  pid_t pid;
  for (size_t cmd_idx = 0; cmd_idx < cmdc; cmd_idx++) {
    if (cmd_pipes[cmd_idx]) {
      close(pipes[cmd_idx][0]);
      close(pipes[cmd_idx][1]);
    }
    pid = pids[cmd_idx];
    if (pid == BUILTIN_PID_VALUE) {
      continue;
    }
    int status;
    waitpid(pid, &status, 0);
    exit_codes[cmd_idx] =
        WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
  }
  return exit_codes[cmdc - 1];
}

static int execute_command(const size_t argc, const char **argv,
                           pid_t *pid_ptr) {
  *pid_ptr = BUILTIN_PID_VALUE;
  const char *first_word = argv[0];
  if (strcmp(first_word, "exit") == 0) {
    return builtin_exit(argc, argv);
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
  if (strcmp(first_word, "complete") == 0) {
    return builtin_complete(argc, argv);
  }
  return try_run_external_program(argc, argv, pid_ptr);
}

static int try_run_external_program(const size_t argc, const char **argv,
                                    pid_t *pid_ptr) {
  char *full_path = get_full_path(argv[0]);
  if (full_path == NULL) {
    fprintf(stderr, "%s: command not found\n", argv[0]);
    return EXIT_FAILURE;
  }
  return run_external_program(argc, argv, full_path, pid_ptr);
}

int run_external_program(const size_t argc, const char **argv,
                         const char *program_path, pid_t *pid_ptr) {
  int argv_copy_length = argc + 1;  // +1 for NULL terminator
  char **argv_copy = malloc(argv_copy_length * sizeof(*argv_copy));
  if (!argv_copy) {
    perror("malloc");
    return -1;
  }
  memcpy(argv_copy, argv, argc * sizeof(*argv_copy));
  argv_copy[argc] = NULL;

  pid_t pid = fork();
  switch (pid) {
  case -1:
    perror("fork");
    return EXIT_FAILURE;
  case 0:
    execv(program_path, argv_copy);
    fprintf(stderr, "%s: '%s': %s", PROGRAM_NAME, program_path,
            strerror(errno));
    // https://stackoverflow.com/a/2329754
    _Exit(EXIT_FAILURE);
  default:
    *pid_ptr = pid;
    return EXIT_SUCCESS;
  }
}
