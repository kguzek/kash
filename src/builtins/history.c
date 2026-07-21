// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/history.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/config.h"
#include "src/lib/history.h"
#include "src/lib/strings.h"

int builtin_history(size_t argc, const char *argv[argc]) {
  size_t entry_start_idx = 0;
  if (argc == 1) {
    return print_history_list(NULL);
  }
  const char *command_name = argv[0];
  const char *option = argv[1];
  if (strcmp(option, "-r") == 0) {
    if (argc != 3) {
      fprintf(stderr, "%s: %s: %s: expected exactly one filename argument",
              PROGRAM_NAME, command_name, option);
      return EXIT_FAILURE;
    }
    const char *path = argv[2];
    int result = read_history_from_file(path);
    if (result != EXIT_SUCCESS) {
      fprintf(stderr, "%s: %s: %s: %s\n", PROGRAM_NAME, command_name, path,
              strerror(errno));
    }
    return result;
  }
  if (strcmp(option, "-w") == 0) {
    if (argc != 3) {
      fprintf(stderr, "%s: %s: %s: expected exactly one filename argument",
              PROGRAM_NAME, command_name, option);
      return EXIT_FAILURE;
    }
    const char *path = argv[2];
    int result = write_history_to_file(path);
    if (result != EXIT_SUCCESS) {
      fprintf(stderr, "%s: %s: %s: %s\n", PROGRAM_NAME, command_name, path,
              strerror(errno));
    }
    return result;
  }
  if (argc > 2) {
    fprintf(stderr,
            "%s: %s: expected at most one optional argument, instead "
            "received %lu\n",
            PROGRAM_NAME, command_name, argc - 1);
    return EXIT_FAILURE;
  }
  long result = strtol(option, (char **)NULL, 10);
  if (result == 0 && !is_zero(option)) {
    fprintf(stderr, "%s: %s: invalid option '%s'\n", PROGRAM_NAME, command_name,
            option);
    return EXIT_FAILURE;
  }
  if (result < 0) {
    fprintf(stderr,
            "%s: %s: expected a nonnegative number, instead received '%s'\n",
            PROGRAM_NAME, command_name, option);
    return EXIT_FAILURE;
  }
  return print_history_list(&result);
}

static int print_history_list(long *last_n_entries) {
  HIST_ENTRY **history = get_history_entries();
  size_t history_size = history == NULL ? 0 : get_history_size();
  size_t entry_start_idx =
      last_n_entries == NULL || *last_n_entries > history_size
          ? 0
          : history_size - *last_n_entries;
  HIST_ENTRY *entry;
  for (size_t i = entry_start_idx; i < history_size; i++) {
    entry = history[i];
    if (entry == NULL) {
      continue;
    }
    printf("    %lu  %s\n", i + 1, entry->line);
  }
  return EXIT_SUCCESS;
}
