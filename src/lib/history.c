// Copyright (c) 2026 Konrad Guzek

#include "src/lib/history.h"

#include <stdlib.h>

static size_t last_append_history_size = 0;

HIST_ENTRY **get_history_entries() {
  return history_list();
}

size_t get_history_size() {
  HISTORY_STATE *history_state = history_get_history_state();
  return history_state == NULL ? 0 : history_state->length;
}

int push_history_entry(const char *cmd) {
  add_history(cmd);
  return EXIT_SUCCESS;
}

int read_history_from_file(const char *path) {
  return read_history(path);
}

int write_history_to_file(const char *path) {
  return write_history(path);
}

int append_history_to_file(const char *path) {
  size_t history_size = get_history_size();
  size_t lines = history_size > last_append_history_size
                     ? history_size - last_append_history_size
                     : 0;
  last_append_history_size = history_size;
  return append_history(lines, path);
}
