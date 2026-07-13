// Copyright (c) 2026 Konrad Guzek

#include "src/path.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char *get_full_path(const char *command) {
  char *path = getenv("PATH");
  if (!path) {
    return NULL;
  }

  char *path_copy = strdup(path);
  char *saveptr = NULL;
  char *path_dir = strtok_r(path_copy, ":", &saveptr);
  char full_path[MAX_PATH_SIZE];
  while (path_dir) {
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

int try_run_external_program(const int argc, const char **argv) {
  char *full_path = get_full_path(argv[0]);
  if (full_path == NULL) {
    fprintf(stderr, "%s: command not found\n", argv[0]);
    return 1;
  }
  return run_external_program(argc, argv, full_path);
}

static int run_external_program(const int argc, const char **argv,
                                const char *program_path) {
  int argv_copy_length = argc + 1;  // +1 for NULL terminator
  char **argv_copy = malloc(argv_copy_length * sizeof(*argv_copy));
  if (!argv_copy) {
    perror("malloc");
    return -1;
  }
  memcpy(argv_copy, argv, argc * sizeof(*argv_copy));
  argv_copy[argc] = NULL;

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return EXIT_FAILURE;
  }
  if (pid == 0) {
    execv(program_path, argv_copy);
  } else {
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
      return WEXITSTATUS(status);
    }
    return 2;
  }
  return EXIT_SUCCESS;
}
