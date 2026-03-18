// Copyright 2026 Konrad Guzek

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void collect_input(char **input, size_t *size) {
  *size = 0;

  printf("$ ");

  getline(input, size, stdin);

  // remove trailing newline
  size_t len = strlen(*input);
  if (len > 0 && (*input)[len - 1] == '\n') {
    (*input)[len - 1] = '\0';
  }
}

const char *BUILTIN_COMMANDS[] = {"exit", "echo", "type"};
const int BUILTIN_COMMANDS_LENGTH =
    sizeof(BUILTIN_COMMANDS) / sizeof(BUILTIN_COMMANDS[0]);

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  char *input = NULL;
  size_t size = 0;
  while (true) {
    collect_input(&input, &size);
    if (strcmp(input, "exit") == 0) {
      break;
    }
    if (strncmp(input, "echo", 4) == 0) {
      if (strlen(input) > 5) {
        printf("%s", input + 5);
      }
    } else if (strncmp(input, "type", 4) == 0) {
      if (strlen(input) > 5) {
        const char *command_match = NULL;
        for (int i = 0; i < BUILTIN_COMMANDS_LENGTH; i++) {
          const char *command = BUILTIN_COMMANDS[i];
          if (strcmp(command, input + 5) == 0) {
            command_match = command;
            break;
          }
        }
        if (command_match == NULL) {
          printf("%s: not found", input + 5);
        } else {
          printf("%s is a shell builtin", command_match);
        }
      } else {
        printf(": not found");
      }
    } else {
      printf("%s: command not found", input);
    }
    printf("\n");
  }

  free(input);
  return 0;
}
