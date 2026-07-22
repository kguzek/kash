// Copyright (c) 2026 Konrad Guzek

#include "src/repl/io.h"

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/history.h>
#include <readline/readline.h>

#include "src/lib/history.h"
#include "src/lib/jobs.h"
#include "src/repl/completions.h"

int prepare_input() {
  using_history();
  int result = rl_bind_key('\t', autocomplete);
  if (result != EXIT_SUCCESS) {
    return result;
  }
  result = restore_history();
  return result;
}

static char *read_single_line() {
  char *line = NULL;
  size_t n = 0;
  ssize_t len = getline(&line, &n, stdin);
  if (len == -1) {
    return NULL;
  }
  return line;
}

size_t collect_input(char **input) {
  print_updated_jobs_list(false);

  char *result =
      isatty(STDIN_FILENO) == 1 ? readline("$ ") : read_single_line();
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
