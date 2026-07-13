// Copyright 2026 Konrad Guzek

#include "src/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <readline/readline.h>

#include "src/completions.h"

#define MAX_SIZE 1024

size_t collect_input(char **input) {
  rl_bind_key('\t', autocomplete);
  char *result = readline("$ ");
  if (result == NULL) {
    *input = NULL;
    return 0;
  }
  *input = result;

  // remove trailing newline
  size_t len = strlen(*input);
  if (len > 0 && (*input)[len - 1] == '\n') {
    (*input)[len - 1] = '\0';
  }
  return len;
}

char *get_full_path(const char *command) {
  char *path = getenv("PATH");
  if (!path) {
    return NULL;
  }

  char *path_copy = strdup(path);
  char *saveptr = NULL;
  char *path_dir = strtok_r(path_copy, ":", &saveptr);
  char full_path[MAX_SIZE];
  while (path_dir) {
    snprintf(full_path, sizeof(full_path), "%s/%s", path_dir, command);
    if (access(full_path, X_OK) == 0) {  // checks if executable
      free(path_copy);
      return strdup(full_path);
    }
    path_dir = strtok_r(NULL, ":", &saveptr);
  }
  free(path_copy);

  return NULL;
}

int split_string(const char *input, char *output[], int max_output_length) {
  const char *delimiter = " ";

  char *input_copy = strdup(input);
  if (!input_copy) {
    perror("strdup");
    return -1;
  }
  int output_length = 0;

  char *saveptr;
  char *token = strtok_r(input_copy, delimiter, &saveptr);
  while (token != NULL && output_length < max_output_length) {
    output[output_length++] = token;
    token = strtok_r(NULL, delimiter, &saveptr);
  }

  return output_length;
}

int run_external_program(char *program_name, const char *program_path,
                         const char *args) {
  char *args_copy = strdup(args);
  if (!args_copy) {
    perror("strdup");
    return -1;
  }

  int max_args_length = 10;
  char *argv[max_args_length + 2];
  argv[0] = program_name;
  int args_length = split_string(args_copy, argv + 1, max_args_length);
  if (args_length < 0) {
    perror("split_string");
    return -1;
  }
  argv[args_length + 1] = NULL;

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return 1;
  }
  if (pid == 0) {
    execv(program_path, argv);
  } else {
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
      return WEXITSTATUS(status);
    }
    return 2;
  }
  return 0;
}

int try_run_external_program(char *first_word, const char *args) {
  char *full_path = get_full_path(first_word);
  if (full_path == NULL) {
    fprintf(stderr, "%s: command not found\n", first_word);
    return 1;
  }
  return run_external_program(first_word, full_path, args);
}

int ring_bell() { return printf("\x07"); }
