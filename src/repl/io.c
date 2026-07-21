// Copyright (c) 2026 Konrad Guzek

#include "src/repl/io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/history.h>
#include <readline/readline.h>

#include "src/lib/history.h"
#include "src/lib/jobs.h"
#include "src/repl/completions.h"

char *prepare_input() {
  char *input = NULL;
  using_history();
  restore_history();
  collect_input(&input);
  return input;
}

size_t collect_input(char **input) {
  rl_bind_key('\t', autocomplete);
  print_updated_jobs_list(false);
  char *result = readline("$ ");
  if (result == NULL) {
    *input = NULL;
    return 0;
  }
  *input = result;
  push_history_entry(result);

  // remove trailing newline
  size_t len = strlen(*input);
  if (len > 0 && (*input)[len - 1] == '\n') {
    (*input)[len - 1] = '\0';
  }
  return len;
}

int reset_output() {
  freopen("/dev/tty", "w", stdout);
  freopen("/dev/tty", "w", stderr);
  return EXIT_SUCCESS;
}
