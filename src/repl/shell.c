// Copyright (c) 2026 Konrad Guzek

#include "src/repl/shell.h"

#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/lib/config.h"
#include "src/lib/history.h"
#include "src/lib/vector.h"
#include "src/repl/executor.h"
#include "src/repl/io.h"
#include "src/repl/parser.h"

sigjmp_buf ctrlc_buf;

int interrupt_input() {
  write(STDOUT_FILENO, "\n", 1);
  siglongjmp(ctrlc_buf, 1);
  return EXIT_SUCCESS;
}

static void sigint_handler(int signo) {
  switch (signo) {
  case SIGINT:
    interrupt_input();
    break;
  default:
    break;
  }
}

int loop() {
  if (signal(SIGINT, sigint_handler) == SIG_ERR) {
    fprintf(stderr, "%s: sigint handler: %s\n", PROGRAM_NAME, strerror(errno));
  }
  prepare_input();
  char *input = NULL;
  while (true) {
    while (sigsetjmp(ctrlc_buf, 1) != 0) {}
    collect_input(&input);
    if (input == NULL) {
      break;
    }
    process_input(input);
    free(input);
  }
  save_history();
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
  int function_result = push_back_size_t(&argc_vec, 0);
  if (function_result != EXIT_SUCCESS) {
    return function_result;
  }
  function_result = calculate_cmdc(input, &cmdc, &argc_vec, NULL);
  if (function_result != EXIT_SUCCESS) {
    free(argc_vec);
    return function_result;
  }
  if (cmdc == 0) {
    free(argc_vec);
    return EXIT_SUCCESS;
  }
  enum COMMAND_SEPARATOR cmd_separators[cmdc];
  size_t *argcv = argc_vec->value;
  char ***cmdv = allocate_cmdv(cmdc, argcv, input, cmd_separators);
  if (cmdv == NULL) {
    free(argc_vec);
    return EXIT_FAILURE;
  }
  function_result =
      execute_commands(cmdc, (const char ***)cmdv, cmd_separators, argcv);
  for (size_t cmdi = 0; cmdi < cmdc; cmdi++) {
    for (size_t argi = 0; argi < argcv[cmdi]; argi++) {
      free(cmdv[cmdi][argi]);
    }
  }
  free(cmdv);
  free(argc_vec);
  if (redirection != NULL) {
    reset_output();
  }
  return function_result;
}
