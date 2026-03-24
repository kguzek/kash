// Copyright 2026 Konrad Guzek

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_SIZE 1024

size_t collect_input(char **input, size_t *size) {
  *size = 0;

  printf("$ ");

  getline(input, size, stdin);

  // remove trailing newline
  size_t len = strlen(*input);
  if (len > 0 && (*input)[len - 1] == '\n') {
    (*input)[len - 1] = '\0';
  }
  return len;
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
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", path_dir, command);
    if (access(full_path, X_OK) == 0) {  // checks if executable
      free(path_copy);
      return strdup(full_path);
    }
    path_dir = strtok_r(NULL, ":", &saveptr);
  }
  free(path_copy);

  return NULL;
}

int split_string(const char *input, char *output[], int max_output_length) {
  const char *delimiter = " ";

  char *input_copy = strdup(input);
  if (!input_copy) {
    perror("strdup");
    return -1;
  }
  int output_length = 0;

  char *saveptr;
  char *token = strtok_r(input_copy, delimiter, &saveptr);
  while (token != NULL && output_length < max_output_length) {
    output[output_length++] = token;
    token = strtok_r(NULL, delimiter, &saveptr);
  }

  return output_length;
}

int run_external_program(char *program_name, const char *program_path,
                         const char *args) {
  char *args_copy = strdup(args);
  if (!args_copy) {
    perror("strdup");
    return -1;
  }

  int max_args_length = 10;
  char *argv[max_args_length + 2];
  argv[0] = program_name;
  int args_length = split_string(args_copy, argv + 1, max_args_length);
  if (args_length < 0) {
    perror("split_string");
    return -1;
  }
  argv[args_length + 1] = NULL;

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return 1;
  }
  if (pid == 0) {
    execv(program_path, argv);
  } else {
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
      return WEXITSTATUS(status);
    }
    return 2;
  }
  return 0;
}

static char command_buf[MAX_SIZE];

char *pwd() { return getcwd(command_buf, MAX_SIZE); }

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
    first_word = strtok(input, " ");
    if (first_word == NULL) {
      continue;
    }
    size_t first_word_length = strlen(first_word);
    args = first_word + first_word_length;
    if (input_length > first_word_length + 1) {
      args++;
    }
    if (strcmp(first_word, "exit") == 0) {
      break;
    } else if (strcmp(first_word, "echo") == 0) {
      printf("%s", args);
    } else if (strcmp(first_word, "type") == 0) {
      if (*args != '\0') {
        const char *command_match = NULL;
        for (int i = 0; i < BUILTIN_COMMANDS_LENGTH; i++) {
          const char *command = BUILTIN_COMMANDS[i];
          if (strcmp(command, args) == 0) {
            command_match = command;
            break;
          }
        }
        if (command_match != NULL) {
          printf("%s is a shell builtin", command_match);
        } else {
          char *full_path = get_full_path(args);
          if (full_path == NULL) {
            printf("%s: not found", args);
          } else {
            printf("%s is %s", args, full_path);
          }
        }
      } else {
        printf(": not found");
      }
    } else if (strcmp(first_word, "pwd") == 0) {
      printf("%s", pwd());
    } else {
      char *full_path = get_full_path(first_word);
      if (full_path == NULL) {
        printf("%s: command not found", first_word);
      } else {
        run_external_program(first_word, full_path, args);
        continue;
      }
    }
    printf("\n");
  }

  free(input);
  return 0;
}
