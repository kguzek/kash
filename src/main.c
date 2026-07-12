// Copyright 2026 Konrad Guzek

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/builtins.h"
#include "src/utils.h"

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  char *input = NULL;
  char *first_word;
  char *args;
  size_t size = 0;
  size_t input_length;
  while (true) {
    input_length = collect_input(&input, &size);
    if (strncmp(input, "exit", 4) == 0) {
      break;
    }
    char *redirection = strstr(input, ">");
    if (redirection != NULL) {
      char *fd_input = redirection;
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
      input_length = fd_input - input;
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
        continue;
      }
      freopen(redirection, file_mode, output_file);
    }
    first_word = strtok(input, " ");
    if (first_word == NULL) {
      continue;
    }
    size_t first_word_length = strlen(first_word);
    args = first_word + first_word_length;
    if (input_length > first_word_length + 1) {
      args++;
    }
    if (strcmp(first_word, "echo") == 0) {
      builtin_echo(args);
    } else if (strcmp(first_word, "type") == 0) {
      builtin_type(args);
    } else if (strcmp(first_word, "pwd") == 0) {
      builtin_pwd(args);
    } else if (strcmp(first_word, "cd") == 0) {
      builtin_cd(args);
    } else {
      try_run_external_program(first_word, args);
    }
    if (redirection != NULL) {
      freopen("/dev/tty", "w", stdout);
    }
  }

  free(input);
  return 0;
}
