// Copyright 2026 Konrad Guzek

#include "src/builtins.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/utils.h"

static const char *BUILTIN_COMMANDS[] = {"exit", "echo", "type", "pwd", "cd"};
static const int BUILTIN_COMMANDS_LENGTH =
    sizeof(BUILTIN_COMMANDS) / sizeof(BUILTIN_COMMANDS[0]);

int builtin_echo(const char *args) {
  printf("%s\n", args);
  return 0;
}

int builtin_type(const char *args) {
  if (*args == '\0') {
    fprintf(stderr, ": not found\n");
    return 2;
  }
  const char *command_match = NULL;
  for (int i = 0; i < BUILTIN_COMMANDS_LENGTH; i++) {
    const char *command = BUILTIN_COMMANDS[i];
    if (strcmp(command, args) == 0) {
      command_match = command;
      break;
    }
  }
  if (command_match != NULL) {
    printf("%s is a shell builtin\n", command_match);
    return 0;
  }
  char *full_path = get_full_path(args);
  if (full_path == NULL) {
    fprintf(stderr, "%s: not found\n", args);
    return 1;
  }
  printf("%s is %s\n", args, full_path);
  return 0;
}

int builtin_pwd(const char *args) {
  char *cwd = getcwd(NULL, 0);
  if (cwd == NULL) {
    perror("pwd");
    return 1;
  }
  printf("%s\n", cwd);
  free(cwd);
  return 0;
}

int builtin_cd(char *args) {
  char *chdir_target = args;
  int result;
  if (*args == '~') {
    chdir_target = getenv("HOME");
    if (chdir_target == NULL) {
      fprintf(stderr, "cd: failed to get home directory\n");
      return 2;
    }
  }
  if (chdir(chdir_target) != 0) {
    fprintf(stderr, "cd: %s: %s\n", chdir_target, strerror(errno));
    return 1;
  }
  return 0;
}
