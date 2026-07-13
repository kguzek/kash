// Copyright 2026 Konrad Guzek

#include "src/completions.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>

#include "src/utils.h"

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

int autocomplete_arguments() {
  char line_buffer_copy[MAX_INPUT_LINE_LENGTH];
  strncpy(line_buffer_copy, rl_line_buffer, sizeof(line_buffer_copy));
  char *first_word = strtok(line_buffer_copy, " ");
  if (first_word == NULL) {
    return EXIT_FAILURE;
  }
  size_t first_word_length = strlen(first_word);
  char *args = line_buffer_copy + first_word_length + 1;
  // printf("\na='%d' b='%d' args='%s'\n$ %s",
  // line_buffer_copy[first_word_length], line_buffer_copy[first_word_length +
  // 1], args, rl_line_buffer);
  int result = autocomplete_filenames(args);
  return result;
}

int autocomplete_builtins() {
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

int autocomplete_external_programs() {
  char *programs[MAX_PATH_SIZE];
  int nprograms = 0;
  int get_result = get_external_programs(programs, &nprograms);
  if (get_result != EXIT_SUCCESS) {
    return get_result;
  }
  return autocomplete_values(programs, nprograms, rl_line_buffer);
}

int autocomplete_filenames(const char *args) {
  char *filenames[MAX_PATH_SIZE];
  int nfilenames = 0;
  int get_result = get_matching_filenames_in_cwd(args, filenames, &nfilenames);
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
    rl_insert_text(" ");
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
    printf("%s ", values[i]);
    free(values[i]);
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
  return EXIT_SUCCESS;
}

static int get_matching_filenames_in_cwd(const char *prefix, char **filenames,
                                         int *nfilenames) {
  DIR *d;
  struct dirent *dir;
  d = opendir(".");
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      switch (dir->d_type) {
      case DT_REG:
      case DT_LNK:
        break;
      default:
        continue;
      }
      if (*prefix == '\0'
          || strncmp(dir->d_name, prefix, strlen(prefix)) == 0) {
        char *filename = strdup(dir->d_name);
        filenames[(*nfilenames)++] = filename;
      }
    }
  }
  closedir(d);
  return EXIT_SUCCESS;
}
