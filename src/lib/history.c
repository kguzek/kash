// Copyright (c) 2026 Konrad Guzek

#include "src/lib/history.h"

#include <stdlib.h>
#include <string.h>

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
