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
#include "src/repl/parser.h"

char *PREVIOUS_AUTOCOMPLETE_INPUT = NULL;

int autocomplete(int count, int key) {
  size_t cmdc;
  struct size_t_vec *argc_vec;
  int function_result = push_back_size_t(&argc_vec, 0);
  if (function_result != EXIT_SUCCESS) {
    return function_result;
  }
  char *input = strdup(rl_line_buffer);
  if (input == NULL) {
    perror("strdup");
    free(argc_vec);
    return EXIT_FAILURE;
  }
  // TODO: use ctx_out not NULL
  int result = calculate_cmdc(input, &cmdc, &argc_vec, NULL);
  if (result != EXIT_SUCCESS) {
    ring_bell();
    free(argc_vec);
    free(input);
    return result;
  }
  bool *cmd_pipes = malloc(cmdc * sizeof(*cmd_pipes));
  if (cmd_pipes == NULL) {
    perror("malloc");
    free(argc_vec);
    free(input);
    return EXIT_FAILURE;
  }
  size_t *argcv = argc_vec->value;
  const char ***cmdv = allocate_cmdv(cmdc, argcv, input, cmd_pipes);
  if (cmdv == NULL) {
    free(argc_vec);
    free(input);
    free(cmd_pipes);
    return EXIT_FAILURE;
  }

  if (cmdc == 0) {
    result = autocomplete_commands("");
  } else {
    // TODO(kguzek): allow completions at any cursor position; assuming last cmd
    size_t cmdi = cmdc - 1;
    const size_t argc = argcv[cmdi];
    const char **argv = cmdv[cmdi];
    switch (argc) {
    case 0:
      result = autocomplete_commands("");
      break;
    case 1:
      result = autocomplete_commands(argv[0]);
      break;
    default:
      result = autocomplete_arguments(argc, argv);
      break;
    }
  }

  if (result != EXIT_SUCCESS) {
    ring_bell();
  }
  return result;
}

static int autocomplete_commands(const char *cmd) {
  int result = autocomplete_builtins(cmd);
  if (result != EXIT_SUCCESS) {
    result = autocomplete_externals(cmd);
  }
  return result;
}

static int autocomplete_arguments(const size_t argc, const char **argv) {
  size_t argi = argc - 1;
  return autocomplete_filenames(argv[argi]);
}

static int autocomplete_builtins(const char *cmd) {
  if (strcmp(cmd, "ech") == 0) {
    rl_insert_text("o ");
    return EXIT_SUCCESS;
  }
  if (strcmp(cmd, "exi") == 0) {
    rl_insert_text("t ");
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}

static int autocomplete_externals(const char *cmd) {
  struct string_vec *externals = NULL;
  int result = get_matching_externals(&externals, cmd);
  if (result != EXIT_SUCCESS) {
    return result;
  }
  result = insert_completions(externals, cmd);
  size_t externals_size = string_vec_size(externals);
  if (externals_size > 0) {
    for (size_t i = 0; i < externals_size; i++) {
      free(externals->value[i]);
    }
    free(externals);
  }
  return result;
}

static int autocomplete_filenames(const char *current_token) {
  struct string_vec *filenames = NULL;
  int result = get_matching_filenames(&filenames, current_token);
  if (result != EXIT_SUCCESS) {
    return result;
  }
  result = insert_completions(filenames, current_token);

  size_t filenames_size = string_vec_size(filenames);
  if (filenames_size > 0) {
    for (size_t i = 0; i < filenames_size; i++) {
      free(filenames->value[i]);
    }
    free(filenames);
  }
  return result;
}

static int insert_completions(struct string_vec *completions,
                              const char *current_token) {
  int completions_size = string_vec_size(completions);
  if (completions_size == 0) {
    return EXIT_FAILURE;  // no matching completions found
  }
  size_t current_token_length = strlen(current_token);
  char *first_completion = completions->value[0];
  if (completions_size == 1) {
    rl_insert_text(first_completion + current_token_length);
    if (first_completion[strlen(first_completion) - 1] != '/') {
      rl_insert_text(" ");
    }
    if (PREVIOUS_AUTOCOMPLETE_INPUT != NULL) {
      free(PREVIOUS_AUTOCOMPLETE_INPUT);
      PREVIOUS_AUTOCOMPLETE_INPUT = strdup(rl_line_buffer);
    }
    return EXIT_SUCCESS;  // completed non-ambiguous value
  }
  char *prefix = get_longest_common_prefix(completions);
  if (prefix != NULL) {
    if (strcmp(current_token, prefix) != 0) {
      rl_insert_text(prefix + current_token_length);
      free(prefix);
      // inserted partial completion, but list is still ambiguous
      return EXIT_FAILURE_EXHAUSTIVE;
    }
    free(prefix);
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
  qsort(completions->value, completions_size, sizeof(first_completion),
        compare_strings);
  for (size_t i = 0; i < completions_size; i++) {
    char *value = completions->value[i];
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
  }
  printf("\n$ %s", rl_line_buffer);
  return EXIT_SUCCESS;  // completion list printed
}

static int get_matching_externals(struct string_vec **externals,
                                  const char *current_token) {
  char *path = getenv("PATH");
  if (!path) {
    return EXIT_FAILURE;
  }
  char *path_copy = strdup(path);
  char *saveptr = NULL;
  char *path_dir = strtok_r(path_copy, ":", &saveptr);
  char full_path[MAX_PATH_SIZE];
  size_t current_token_length = strlen(current_token);
  while (path_dir) {
    DIR *d;
    struct dirent *dir;
    d = opendir(path_dir);
    if (d != NULL) {
      while ((dir = readdir(d)) != NULL) {
        if (dir->d_type != DT_REG) {
          continue;
        }
        snprintf(full_path, sizeof(full_path), "%s/%s", path_dir, dir->d_name);
        if (access(full_path, X_OK) != 0) {
          continue;
        }
        if (current_token_length == 0
            || strncmp(dir->d_name, current_token, current_token_length) == 0) {
          char *filename = strdup(dir->d_name);
          push_back_string_unique(externals, filename);
        }
      }
      closedir(d);
    }
    path_dir = strtok_r(NULL, ":", &saveptr);
  }
  free(path_copy);
  return EXIT_SUCCESS;
}

static int get_matching_filenames(struct string_vec **filenames,
                                  const char *current_token) {
  DIR *d;
  struct dirent *dir;
  const char *filename_prefix = strrchr(current_token, '/');
  char *dir_path;
  bool is_cwd;
  if (filename_prefix == NULL) {
    dir_path = ".";
    is_cwd = true;
    filename_prefix = current_token;
  } else {
    dir_path = strndup(current_token, filename_prefix - current_token);
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
          || strncmp(dir->d_name, filename_prefix, strlen(filename_prefix))
                 == 0) {
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
        push_back_string_unique(filenames, filename);
      }
    }
  }
  closedir(d);
  return EXIT_SUCCESS;
}
