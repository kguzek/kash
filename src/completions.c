// Copyright 2026 Konrad Guzek

#include "src/completions.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>

#include "src/utils.h"

char *PREVIOUS_AUTOCOMPLETE_INPUT = NULL;

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

int get_external_programs(char *programs[], int *nprograms) {
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
          char *filename = strdup(dir->d_name);
          programs[(*nprograms)++] = filename;
        }
      }
    }
    closedir(d);
    path_dir = strtok_r(NULL, ":", &saveptr);
  }
  free(path_copy);
  return 0;
}

int autocomplete_external_programs() {
  char *programs[MAX_PATH_SIZE];
  int nprograms = 0;
  int get_result = get_external_programs(programs, &nprograms);
  if (get_result != 0) {
    return get_result;
  }
  if (nprograms == 0) {
    return 1;  // no matching completions found
  }
  if (nprograms == 1) {
    rl_insert_text(programs[0] + strlen(rl_line_buffer));
    rl_insert_text(" ");
    free(programs[0]);
    if (PREVIOUS_AUTOCOMPLETE_INPUT != NULL) {
      free(PREVIOUS_AUTOCOMPLETE_INPUT);
      PREVIOUS_AUTOCOMPLETE_INPUT = strdup(rl_line_buffer);
    }
    return 0;  // completed non-ambiguous command
  }
  char *prefix = get_longest_common_prefix(programs, nprograms);
  if (prefix != NULL && strcmp(rl_line_buffer, prefix) != 0) {
    rl_insert_text(prefix + strlen(rl_line_buffer));
    free(prefix);
    return 3;  // completed partial command, list is still ambiguous
  }
  if (PREVIOUS_AUTOCOMPLETE_INPUT == NULL) {
    PREVIOUS_AUTOCOMPLETE_INPUT = strdup(rl_line_buffer);
    return 2;  // command list is ambiguous
  }
  if (strcmp(PREVIOUS_AUTOCOMPLETE_INPUT, rl_line_buffer) != 0) {
    free(PREVIOUS_AUTOCOMPLETE_INPUT);
    PREVIOUS_AUTOCOMPLETE_INPUT = strdup(rl_line_buffer);
    return 2;  // same as above, but this is not the first completion attempt
  }
  printf("\n");
  qsort(programs, nprograms, sizeof(programs[0]), compare_strings);
  for (size_t i = 0; i < nprograms; i++) {
    printf("%s ", programs[i]);
    free(programs[i]);
  }
  printf("\n$ %s", rl_line_buffer);
  return 0;  // completion list printed
}
