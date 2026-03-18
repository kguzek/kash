// Copyright 2026 Konrad Guzek

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

char *get_full_path(const char *command) {
  char *path = getenv("PATH");
  if (!path) {
    return NULL;
  }

  char *path_copy = strdup(path);
  char *saveptr = NULL;
  char *path_dir = strtok_r(path_copy, ":", &saveptr);
  while (path_dir) {
    path_dir = strtok_r(NULL, ":", &saveptr);
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", path_dir, command);
    if (access(full_path, X_OK) == 0) {  // checks if executable
      free(path_copy);
      return strdup(full_path);
    }
  }
  free(path_copy);

  return NULL;
}

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
        const char *command_arg = input + 5;
        const char *command_match = NULL;
        for (int i = 0; i < BUILTIN_COMMANDS_LENGTH; i++) {
          const char *command = BUILTIN_COMMANDS[i];
          if (strcmp(command, command_arg) == 0) {
            command_match = command;
            break;
          }
        }
        if (command_match != NULL) {
          printf("%s is a shell builtin", command_match);
        } else {
          char *full_path = get_full_path(command_arg);
          if (full_path == NULL) {
            printf("%s: not found", command_arg);
          } else {
            printf("%s is %s", command_arg, full_path);
          }
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
