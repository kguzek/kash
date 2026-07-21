// Copyright (c) 2026 Konrad Guzek

#include "src/lib/history.h"

#include <string.h>

#include "src/builtins/history.h"
#include "src/lib/vector.h"

static struct string_vec *history = NULL;

struct string_vec *get_history_entries() {
  return history;
}

int push_history_entry(const char *cmd) {
  return push_back_string(&history, strdup(cmd));
}
