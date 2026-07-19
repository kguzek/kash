// Copyright (c) 2026 Konrad Guzek

#include "src/lib/config.h"

const char *PROGRAM_NAME = "kash";
const char *BUILTIN_COMMANDS[] = {"exit", "echo",     "type", "pwd",
                                  "cd",   "complete", "jobs"};
const int BUILTIN_COMMANDS_LENGTH =
    sizeof(BUILTIN_COMMANDS) / sizeof(BUILTIN_COMMANDS[0]);
