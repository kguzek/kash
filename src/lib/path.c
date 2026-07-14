// Copyright (c) 2026 Konrad Guzek

#include "src/lib/path.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/lib/config.h"

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
