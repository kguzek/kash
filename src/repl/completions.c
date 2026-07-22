// Copyright 2026 Konrad Guzek

#include "src/repl/completions.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <readline/readline.h>

#include "src/lib/config.h"
#include "src/lib/strings.h"
#include "src/lib/terminal.h"
#include "src/lib/vector.h"
#include "src/repl/executor.h"
#include "src/repl/parser.h"

static char *PREVIOUS_AUTOCOMPLETE_INPUT = NULL;

static struct string_pair_vec registered_completion_specs = {NULL, NULL};

int register_completion_spec(const char *cmd, const char *spec_path) {
  return push_back_string_pair(&registered_completion_specs, strdup(cmd),
                               strdup(spec_path));
}

int unregister_completion_spec(const char *cmd) {
  size_t size = string_pair_vec_size(&registered_completion_specs);
  const char *key;
  for (size_t i = 0; i < size; i++) {
    key = string_pair_vec_key(&registered_completion_specs, i);
    if (strcmp(key, cmd) != 0) {
      continue;
    }
    // TODO(kguzek): make this less ugly XD
    registered_completion_specs.keys->value[i] = "";
  }
  return EXIT_SUCCESS;
}

size_t populate_registered_completion_specs(const char *cmd,
                                            struct string_vec **specs) {
  size_t specs_size = string_pair_vec_size(&registered_completion_specs);
  if (specs_size == 0) {
    return 0;
  }
  size_t registered_specs = 0;
  const char *spec_cmd, *spec_path;
  for (size_t i = 0; i < specs_size; i++) {
    spec_cmd = string_pair_vec_key(&registered_completion_specs, i);
    if (strcmp(spec_cmd, cmd) != 0) {
      continue;
    }
    spec_path = string_pair_vec_value(&registered_completion_specs, i);
    push_back_string(specs, strdup(spec_path));
    registered_specs++;
  }
  return registered_specs;
}

int autocomplete(int count, int key) {
  size_t cmdc = 0;
  struct size_t_vec *argc_vec = NULL;
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
  struct cmd_parse_ctx parse_ctx;
  int result = calculate_cmdc(input, &cmdc, &argc_vec, &parse_ctx);
  if (result != EXIT_SUCCESS) {
    ring_bell();
    free(argc_vec);
    free(input);
    return result;
  }
  enum COMMAND_SEPARATOR cmd_separators[cmdc];
  size_t *argcv = argc_vec->value;
  char ***cmdv = allocate_cmdv(cmdc, argcv, input, cmd_separators);
  if (cmdv == NULL) {
    free(argc_vec);
    free(input);
    return EXIT_FAILURE;
  }
  struct string_vec *completions = NULL;
  const char *current_token;

  if (cmdc == 0) {
    current_token = "";
    result = populate_command_completions(&completions, current_token);
  } else {
    // TODO(kguzek): allow completions at any cursor position; assuming last cmd
    size_t cmdi = cmdc - 1;
    const size_t argc = argcv[cmdi];
    const char **argv = (const char **)cmdv[cmdi];
    const char *cmd = argv[0], *previous_token = "";

    if (argc > 0 && (parse_ctx.starting_new_arg)) {
      current_token = "";
      previous_token = argv[argc - 1];
      result = populate_argument_completions(&completions, cmd, current_token,
                                             previous_token);
    } else {
      switch (argc) {
      case 0:
        current_token = "";
        result = populate_command_completions(&completions, current_token);
        break;
      case 1:
        current_token = argv[0];
        result = populate_command_completions(&completions, current_token);
        break;
      default:
        current_token = argv[argc - 1];
        previous_token = argv[argc - 2];
        result = populate_argument_completions(&completions, cmd, current_token,
                                               previous_token);
        break;
      }
    }
  }
  for (size_t cmdi = 0; cmdi < cmdc; cmdi++) {
    for (size_t argi = 0; argi < argcv[cmdi]; argi++) {
      free(cmdv[cmdi][argi]);
    }
  }

  result = insert_completions(completions, &parse_ctx, current_token);

  free_string_vec(completions);
  if (completions != NULL) {
    free(completions);
  }

  if (result != EXIT_SUCCESS) {
    ring_bell();
  }
  return result;
}

static int populate_command_completions(struct string_vec **completions,
                                        const char *cmd) {
  populate_builtin_completions(completions, cmd);
  return populate_external_completions(completions, cmd);
}

static int populate_argument_completions(struct string_vec **completions,
                                         const char *cmd,
                                         const char *current_token,
                                         const char *previous_token) {
  struct string_vec *spec_paths = NULL;
  size_t spec_paths_size =
      populate_registered_completion_specs(cmd, &spec_paths);
  if (spec_paths_size == 0) {
    // fallback to filename completions
    return populate_filename_completions(completions, current_token);
  } else {
    int exit_code = populate_spec_completions(completions, spec_paths, cmd,
                                              current_token, previous_token);
    free_string_vec(spec_paths);
    return exit_code;
  }
}

static int populate_builtin_completions(struct string_vec **completions,
                                        const char *cmd) {
  size_t cmd_length = strlen(cmd);
  const char *builtin;
  for (size_t i = 0; i < BUILTIN_COMMANDS_LENGTH; i++) {
    builtin = BUILTIN_COMMANDS[i];
    if (strncmp(builtin, cmd, cmd_length) == 0) {
      push_back_string(completions, strdup(builtin));
    }
  }
  return EXIT_SUCCESS;
}

static int insert_completions(struct string_vec *completions,
                              struct cmd_parse_ctx *parse_ctx,
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
      if (parse_ctx->in_single_quotes) {
        rl_insert_text("' ");
      } else if (parse_ctx->in_double_quotes) {
        rl_insert_text("\" ");
      } else {
        rl_insert_text(" ");
      }
    }
    if (PREVIOUS_AUTOCOMPLETE_INPUT != NULL) {
      free(PREVIOUS_AUTOCOMPLETE_INPUT);
      PREVIOUS_AUTOCOMPLETE_INPUT = NULL;
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
    const char *hint = filename == NULL ? value : filename + 1;
    const bool hint_needs_escaping = strchr(hint, ' ') != NULL;
    printf(hint_needs_escaping ? "'%s' " : "%s ", hint);
  }
  printf("\n$ %s", rl_line_buffer);
  return EXIT_SUCCESS;  // completion list printed
}

static int populate_external_completions(struct string_vec **externals,
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

static int populate_filename_completions(struct string_vec **filenames,
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

static int populate_spec_completions(struct string_vec **completions,
                                     struct string_vec *spec_paths,
                                     const char *cmd, const char *current_token,
                                     const char *previous_token) {
  size_t spec_paths_size = spec_paths->size;
  int saved_stdin = dup(STDIN_FILENO), saved_stdout = dup(STDOUT_FILENO);
  const char *spec_path;
  for (size_t i = 0; i < spec_paths_size; i++) {
    pid_t pid;
    int pipes[2];
    if (pipe(pipes) == -1) {
      perror("pipe");
      return EXIT_FAILURE;
    }
    spec_path = spec_paths->value[i];
    const char *spec_argv[] = {spec_path, cmd, current_token, previous_token};
    dup2(pipes[1], STDOUT_FILENO);
    struct string_pair_vec spec_envs = {NULL, NULL};
    char comp_point[128];
    snprintf(comp_point, sizeof(comp_point), "%lu", strlen(rl_line_buffer));
    push_back_string_pair(&spec_envs, "COMP_POINT", comp_point);
    push_back_string_pair(&spec_envs, "COMP_LINE", rl_line_buffer);
    int result =
        run_external_program(4, spec_argv, spec_path, &pid, &spec_envs);
    // not freeing because neither keys nor rl_line_buffer are owned by us
    // comp_point lives on the stack I think so it should also not be freed??
    // free_string_pair_vec(&spec_envs);
    close(pipes[1]);
    dup2(saved_stdout, STDOUT_FILENO);
    if (result != EXIT_SUCCESS) {
      close(pipes[0]);
      dup2(saved_stdin, STDIN_FILENO);
      return result;
    }
    char *output = malloc(1);
    size_t output_size = 0;
    size_t max_len = 0;
    char tmp[4096];
    ssize_t n;
    while ((n = read(pipes[0], tmp, sizeof(tmp))) > 0) {
      if (output_size + n + 1 > max_len) {
        max_len = (output_size + n + 1) * 2;
        output = realloc(output, max_len);
      }
      memcpy(output + output_size, tmp, n);
      output_size += n;
    }
    close(pipes[0]);
    dup2(saved_stdin, STDIN_FILENO);
    output[output_size] = '\0';
    char *saveptr = NULL;
    char *completion = strtok_r(output, "\n", &saveptr);
    while (completion != NULL) {
      if (strncmp(completion, current_token, strlen(current_token)) == 0) {
        result = push_back_string_unique(completions, strdup(completion));
      }
      completion = strtok_r(NULL, "\n", &saveptr);
    }
    free(output);
    int status;
    waitpid(pid, &status, 0);
    if (!WIFEXITED(status)) {
      return EXIT_FAILURE;
    }
    result = WEXITSTATUS(status);
    if (result != EXIT_SUCCESS) {
      return result;
    }
  }
  return EXIT_SUCCESS;
}
