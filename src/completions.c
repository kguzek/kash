// Copyright 2026 Konrad Guzek

#include "src/completions.h"

#include <stdio.h>
#include <readline/readline.h>

int autocomplete(int count, int key) {
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
