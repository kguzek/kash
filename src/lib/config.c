// Copyright (c) 2026 Konrad Guzek

#include "src/lib/config.h"

const char *PROGRAM_NAME = "kash";
const char *BUILTIN_COMMANDS[] = {"exit",     "echo", "type",    "pwd",    "cd",
                                  "complete", "jobs", "history", "declare"};

const int BUILTIN_COMMANDS_LENGTH =
    sizeof(BUILTIN_COMMANDS) / sizeof(BUILTIN_COMMANDS[0]);

static int previous_exit_code = 0;

int get_previous_exit_code() {
  return previous_exit_code;
}

int set_previous_exit_code(int new_exit_code) {
  return (previous_exit_code = new_exit_code);
}
