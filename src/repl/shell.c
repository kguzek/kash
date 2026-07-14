// Copyright (c) 2026 Konrad Guzek

#include "src/repl/shell.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/vector.h"
#include "src/repl/executor.h"
#include "src/repl/io.h"
#include "src/repl/parser.h"

int loop() {
  char *input = NULL;
  collect_input(&input);
  while (input != NULL) {
    process_input(input);
    collect_input(&input);
  }

  free(input);
  return EXIT_SUCCESS;
}

static int process_input(char *input) {
  char *redirection = strstr(input, ">");
  int redirection_result = handle_redirection(input, redirection);
  if (redirection_result != EXIT_SUCCESS) {
    return redirection_result;
  }
  size_t cmdc = 0;
  struct size_t_vec *argc_vec = NULL;
  int function_result = push_back(&argc_vec, 0);
  if (function_result != EXIT_SUCCESS) {
    return function_result;
  }
  function_result = calculate_cmdc(input, &cmdc, &argc_vec);
  if (function_result != EXIT_SUCCESS) {
    free(argc_vec);
    return function_result;
  }
  if (cmdc == 0) {
    free(argc_vec);
    return EXIT_SUCCESS;
  }
  bool *cmd_pipes = malloc(cmdc * sizeof(*cmd_pipes));
  if (cmd_pipes == NULL) {
    perror("malloc");
    free(argc_vec);
    return EXIT_FAILURE;
  }
  size_t *argcv = argc_vec->value;
  const char ***cmdv = allocate_cmdv(cmdc, argcv, input, cmd_pipes);
  if (cmdv == NULL) {
    free(cmd_pipes);
    return EXIT_FAILURE;
  }
  function_result = execute_commands(cmdc, cmdv, cmd_pipes, argcv);
  free(cmd_pipes);
  free(argc_vec);
  if (redirection != NULL) {
    reset_output();
  }
  return function_result;
}
