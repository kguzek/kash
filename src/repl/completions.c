// Copyright 2026 Konrad Guzek

#include "src/repl/completions.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>

#include "src/lib/config.h"
#include "src/lib/strings.h"
#include "src/lib/terminal.h"

char *PREVIOUS_AUTOCOMPLETE_INPUT = NULL;

/* This is a macro because a function wouldn't allow us to return/halt from
 * inside the switch statement*/
#define HANDLE_AUTOCOMPLETE_EXIT(result)                                       \
  switch (result) {                                                            \
  case EXIT_SUCCESS:                                                           \
    return EXIT_SUCCESS;                                                       \
  case EXIT_FAILURE_EXHAUSTIVE:                                                \
    ring_bell();                                                               \
    return EXIT_FAILURE_EXHAUSTIVE;                                            \
  default:                                                                     \
    break;                                                                     \
  }

int autocomplete(int count, int key) {
  HANDLE_AUTOCOMPLETE_EXIT(autocomplete_builtins());
  HANDLE_AUTOCOMPLETE_EXIT(autocomplete_external_programs())
  HANDLE_AUTOCOMPLETE_EXIT(autocomplete_arguments())
  ring_bell();
  return EXIT_FAILURE;
}

static int autocomplete_arguments() {
  char line_buffer_copy[MAX_INPUT_LINE_LENGTH];
  strncpy(line_buffer_copy, rl_line_buffer, sizeof(line_buffer_copy));
  char *args = strrchr(line_buffer_copy, ' ');
  if (args == NULL) {
    // no arguments to complete
    return EXIT_FAILURE;
  }
  int result = autocomplete_filenames(args + 1);  // remove leading space
  return result;
}

static int autocomplete_builtins() {
  if (strcmp(rl_line_buffer, "ech") == 0) {
    rl_insert_text("o ");
    return EXIT_SUCCESS;
  }
  if (strcmp(rl_line_buffer, "exi") == 0) {
    rl_insert_text("t ");
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}

static int autocomplete_external_programs() {
  char *programs[MAX_PATH_SIZE];
  int nprograms = 0;
  int get_result = get_external_programs(programs, &nprograms);
  if (get_result != EXIT_SUCCESS) {
    return get_result;
  }
  return autocomplete_values(programs, nprograms, rl_line_buffer);
}

static int autocomplete_filenames(const char *args) {
  char *filenames[MAX_PATH_SIZE];
  int nfilenames = 0;
  int get_result = get_matching_filenames(args, filenames, &nfilenames);
  if (get_result != EXIT_SUCCESS) {
    return get_result;
  }
  return autocomplete_values(filenames, nfilenames, args);
}

static int autocomplete_values(char *values[], int nvalues,
                               const char *current_token) {
  if (nvalues == 0) {
    return EXIT_FAILURE;  // no matching completions found
  }
  size_t current_token_length = strlen(current_token);
  if (nvalues == 1) {
    rl_insert_text(values[0] + current_token_length);
    if (values[0][strlen(values[0]) - 1] != '/') {
      rl_insert_text(" ");
    }
    free(values[0]);
    if (PREVIOUS_AUTOCOMPLETE_INPUT != NULL) {
      free(PREVIOUS_AUTOCOMPLETE_INPUT);
      PREVIOUS_AUTOCOMPLETE_INPUT = strdup(rl_line_buffer);
    }
    return EXIT_SUCCESS;  // completed non-ambiguous value
  }
  char *prefix = get_longest_common_prefix(values, nvalues);
  if (prefix != NULL && strcmp(current_token, prefix) != 0) {
    rl_insert_text(prefix + current_token_length);
    free(prefix);
    // inserted partial completion, but list is still ambiguous
    return EXIT_FAILURE_EXHAUSTIVE;
  }
  if (PREVIOUS_AUTOCOMPLETE_INPUT == NULL) {
    PREVIOUS_AUTOCOMPLETE_INPUT = strdup(rl_line_buffer);
    return EXIT_FAILURE_EXHAUSTIVE;  // completion list is ambiguous
  }
  if (strcmp(PREVIOUS_AUTOCOMPLETE_INPUT, rl_line_buffer) != 0) {
    free(PREVIOUS_AUTOCOMPLETE_INPUT);
    PREVIOUS_AUTOCOMPLETE_INPUT = strdup(rl_line_buffer);
    // same as above, but this is not the first completion attempt
    return EXIT_FAILURE_EXHAUSTIVE;
  }
  printf("\n");
  qsort(values, nvalues, sizeof(values[0]), compare_strings);
  for (size_t i = 0; i < nvalues; i++) {
    char *value = values[i];
    size_t last_char_index = strlen(value) - 1;
    bool is_dir = value[last_char_index] == '/';
    if (is_dir) {
      // temporarily remove trailing slash so output includes the directory name
      value[last_char_index] = '\0';
    }
    char *filename = strrchr(value, '/');
    if (is_dir) {
      value[last_char_index] = '/';
    }
    printf("%s ", filename == NULL ? value : filename + 1);
    free(value);
  }
  printf("\n$ %s", rl_line_buffer);
  return EXIT_SUCCESS;  // completion list printed
}

static int get_external_programs(char *programs[], int *nprograms) {
  char *path = getenv("PATH");
  if (!path) {
    return EXIT_FAILURE;
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
        if (strcmp(dir->d_name, rl_line_buffer) == 0) {
          char *filename = strdup(dir->d_name);
          programs[(*nprograms)++] = filename;
        }
      }
    }
    closedir(d);
    path_dir = strtok_r(NULL, ":", &saveptr);
  }
  free(path_copy);
  return EXIT_SUCCESS;
}

static int get_matching_filenames(const char *prefix, char **filenames,
                                  int *nfilenames) {
  DIR *d;
  struct dirent *dir;
  const char *filename_prefix = strrchr(prefix, '/');
  char *dir_path;
  bool is_cwd;
  if (filename_prefix == NULL) {
    dir_path = ".";
    is_cwd = true;
    filename_prefix = prefix;
  } else {
    dir_path = strndup(prefix, filename_prefix - prefix);
    is_cwd = false;
    filename_prefix++;
  }
  d = opendir(dir_path);
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      switch (dir->d_type) {
      case DT_REG:
      case DT_LNK:
      case DT_DIR:
        break;
      default:
        continue;
      }
      if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
        continue;
      }
      if (*filename_prefix == '\0'
          || strcmp(dir->d_name, filename_prefix) == 0) {
        char *filename;
        bool is_dir = dir->d_type == DT_DIR;
        size_t path_len = strlen(dir->d_name) + (is_dir ? 2 : 1);
        if (is_cwd) {
          filename = malloc(path_len);
          if (filename) {
            snprintf(filename, path_len, is_dir ? "%s/" : "%s", dir->d_name);
          }
        } else {
          path_len += strlen(dir_path) + 1;
          filename = malloc(path_len);
          if (filename) {
            snprintf(filename, path_len, is_dir ? "%s/%s/" : "%s/%s", dir_path,
                     dir->d_name);
          }
        }
        filenames[(*nfilenames)++] = filename;
      }
    }
  }
  closedir(d);
  return EXIT_SUCCESS;
}
