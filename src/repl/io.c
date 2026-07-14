// Copyright (c) 2026 Konrad Guzek

#include "src/repl/io.h"

#include <stdio.h>
#include <readline/readline.h>

#include "src/repl/completions.h"

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
