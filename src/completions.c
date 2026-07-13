// Copyright 2026 Konrad Guzek

#include "src/completions.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>

#include "src/utils.h"

int autocomplete(int count, int key) {
  if (autocomplete_builtins() == 0) {
    return 0;
  }
  if (autocomplete_external_programs() == 0) {
    return 0;
  }
  ring_bell();
  return 1;
}

int autocomplete_builtins() {
  if (strcmp(rl_line_buffer, "ech") == 0) {
    rl_insert_text("o ");
    return 0;
  }
  if (strcmp(rl_line_buffer, "exi") == 0) {
    rl_insert_text("t ");
    return 0;
  }
  return 1;
}

int autocomplete_external_programs() {
  char *path = getenv("PATH");
  if (!path) {
    return 1;
  }

  char *path_copy = strdup(path);
  char *saveptr = NULL;
  char *path_dir = strtok_r(path_copy, ":", &saveptr);
  char full_path[MAX_PATH_SIZE];
  while (path_dir) {
    DIR *d;
    struct dirent *dir;
    d = opendir(path_dir);
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        if (dir->d_type != DT_REG) {
          continue;
        }
        snprintf(full_path, sizeof(full_path), "%s/%s", path_dir, dir->d_name);
        if (access(full_path, X_OK) != 0) {
          continue;
        }
        if (strncmp(dir->d_name, rl_line_buffer, strlen(rl_line_buffer)) == 0) {
          rl_insert_text(dir->d_name + strlen(rl_line_buffer));
          rl_insert_text(" ");
          closedir(d);
          free(path_copy);
          return 0;
        }
      }
    }
    closedir(d);
    path_dir = strtok_r(NULL, ":", &saveptr);
  }
  free(path_copy);
  return 1;
}
