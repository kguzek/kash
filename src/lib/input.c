// Copyright 2026 Konrad Guzek

#include "src/lib/input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>

#include "src/builtins/cd.h"
#include "src/builtins/echo.h"
#include "src/builtins/pwd.h"
#include "src/builtins/type.h"
#include "src/completions.h"
#include "src/lib/path.h"

#define IS_BACKSLASH_ESCAPED                                                   \
  in_single_quotes || next_char_escaped                                        \
      || (in_double_quotes && (!is_escapable_in_double_quotes(*(c + 1))))

static const char *PROGRAM_NAME = "kash";

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
  int argc = calculate_argc(input);
  if (argc == 0) {
    return EXIT_SUCCESS;
  }
  if (argc < 0) {
    return EXIT_FAILURE;
  }
  const char **argv = allocate_argv(argc, input);
  if (argv == NULL) {
    return EXIT_FAILURE;
  }
  const char *first_word = argv[0];
  if (strcmp(first_word, "echo") == 0) {
    builtin_echo(argc, argv);
  } else if (strcmp(first_word, "type") == 0) {
    builtin_type(argc, argv);
  } else if (strcmp(first_word, "pwd") == 0) {
    builtin_pwd(argc, argv);
  } else if (strcmp(first_word, "cd") == 0) {
    builtin_cd(argc, argv);
  } else {
    try_run_external_program(argc, argv);
  }
  if (redirection != NULL) {
    freopen("/dev/tty", "w", stdout);
  }
  return EXIT_SUCCESS;
}

static int calculate_argc(const char *input) {
  size_t argc = 0;
  bool starting_new_arg = true;
  bool in_single_quotes = false;
  bool in_double_quotes = false;
  bool next_char_escaped = false;
  for (const char *c = input; *c != '\0'; c++) {
    const bool char_escaped =
        next_char_escaped || in_single_quotes || in_double_quotes;
    switch (*c) {
    case '\\':
      if (IS_BACKSLASH_ESCAPED) {
        goto handle_other_char;
      }
      next_char_escaped = true;
      break;
    case '\'':
      if (in_double_quotes || next_char_escaped) {
        goto handle_other_char;
      }
      in_single_quotes = !in_single_quotes;
      break;
    case '"':
      if (in_single_quotes || next_char_escaped) {
        goto handle_other_char;
      }
      in_double_quotes = !in_double_quotes;
      break;
    case ' ':
      if (!char_escaped) {
        starting_new_arg = true;
      }
      next_char_escaped = false;
      break;
    default:
    handle_other_char:
      if (starting_new_arg) {
        argc++;
        starting_new_arg = false;
      }
      next_char_escaped = false;
      break;
    }
  }
  if (in_single_quotes) {
    fprintf(stderr, "%s: unmatched '\n", PROGRAM_NAME);
    return -1;
  }
  if (in_double_quotes) {
    fprintf(stderr, "%s: unmatched \"\n", PROGRAM_NAME);
    return -1;
  }
  return argc;
}

static const char **allocate_argv(size_t argc, char *input) {
  const char **argv = malloc(argc * sizeof(*argv));
  if (argv == NULL) {
    perror("malloc");
    return NULL;
  }
  const char *input_copy = strdup(input);
  bool starting_new_arg = true;
  bool in_single_quotes = false;
  bool in_double_quotes = false;
  bool next_char_escaped = false;
  size_t arg_start_idx = 0, arg_idx = 0, input_idx = 0;
  for (const char *c = input_copy; *c != '\0'; c++) {
    const bool char_escaped =
        next_char_escaped || in_single_quotes || in_double_quotes;
    switch (*c) {
    case '\\':
      if (IS_BACKSLASH_ESCAPED) {
        goto copy_char;
      }
      next_char_escaped = true;
      break;
    case '\'':
      if (in_double_quotes || next_char_escaped) {
        goto copy_char;
      }
      in_single_quotes = !in_single_quotes;
      break;
    case '"':
      if (in_single_quotes || next_char_escaped) {
        goto copy_char;
      }
      in_double_quotes = !in_double_quotes;
      break;
    case ' ':
      if (char_escaped) {
        goto copy_char;
      }
      if (starting_new_arg) {
        break;  // repeated space: no-op
      }
      // first unquoted space: copy NULL-terminated arg
      input[input_idx++] = '\0';
      starting_new_arg = true;
      argv[arg_idx++] = input + arg_start_idx;
      arg_start_idx = input_idx;
      break;
    default:
    copy_char:
      input[input_idx++] = *c;
      starting_new_arg = false;
      next_char_escaped = false;
      break;
    }
  }
  if (!starting_new_arg) {
    // ensure final arg is also stored
    input[input_idx++] = '\0';
    argv[arg_idx] = input + arg_start_idx;
  }
  return argv;
}

static const bool is_escapable_in_double_quotes(const char c) {
  return c == '\\' || c == '"';
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
