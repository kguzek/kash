// Copyright 2026 Konrad Guzek

#include "src/lib/input.h"

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>

#include "src/builtins.h"
#include "src/completions.h"
#include "src/lib/path.h"

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

int process_input(char *input) {
  char *redirection = strstr(input, ">");
  int redirection_result = handle_redirection(input, redirection);
  if (redirection_result != EXIT_SUCCESS) {
    return redirection_result;
  }
  size_t args_count = calculate_args_count(input);
  const char **args = allocate_args_array(args_count, input);
  if (args == NULL) {
    return EXIT_FAILURE;
  }
  const char *first_word = args[0];
  if (strcmp(first_word, "echo") == 0) {
    builtin_echo(args_count, args);
  } else if (strcmp(first_word, "type") == 0) {
    builtin_type(args_count, args);
  } else if (strcmp(first_word, "pwd") == 0) {
    builtin_pwd(args_count, args);
  } else if (strcmp(first_word, "cd") == 0) {
    builtin_cd(args_count, args);
  } else {
    try_run_external_program(args_count, args);
  }
  if (redirection != NULL) {
    freopen("/dev/tty", "w", stdout);
  }
  return EXIT_SUCCESS;
}

static size_t calculate_args_count(const char *input) {
  size_t args_count = 0;
  char *input_copy = strdup(input);
  char *saveptr, *token = strtok_r(input_copy, " ", &saveptr);
  for (args_count = 0; token != NULL; args_count++) {
    token = strtok_r(NULL, " ", &saveptr);
  }
  free(input_copy);
  return args_count;
}

static const char **allocate_args_array(size_t args_count, char *input) {
  const char **args = malloc(args_count * sizeof(*args));
  if (args == NULL) {
    perror("malloc");
    return NULL;
  }
  char *saveptr, *token = strtok_r(input, " ", &saveptr);
  for (size_t arg_idx = 0; token != NULL; arg_idx++) {
    args[arg_idx] = token;
    token = strtok_r(NULL, " ", &saveptr);
  }
  return args;
}

static int handle_redirection(char *input, char *redirection) {
  if (redirection == NULL) {
    return EXIT_SUCCESS;
  }
  char *fd_input = redirection;
  if (redirection == input) {
    fprintf(stderr, "Using output redirection at the beginning of the command "
                    "is not implemented\n");
    return EXIT_FAILURE;
  }
  char fd_number = *(fd_input - 1);
  FILE *output_file = stdout;
  char *file_mode = "w";
  if (redirection != input) {
    if (fd_number == '1') {
      fd_input--;
    } else if (fd_number == '2') {
      fd_input--;
      output_file = stderr;
    }
  }
  *fd_input = '\0';
  // input_length = fd_input - input;
  redirection++;
  if (redirection != NULL && *redirection == '>') {
    file_mode = "a";
    redirection++;
  }
  while (*redirection == ' ') {
    redirection++;
  }
  if (*redirection == '\0') {
    fprintf(stderr, "No file specified for redirection\n");
    return EXIT_FAILURE;
  }
  freopen(redirection, file_mode, output_file);
  return EXIT_SUCCESS;
}
