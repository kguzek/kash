// Copyright (c) 2026 Konrad Guzek

#include "src/lib/config.h"

const char *BUILTIN_COMMANDS[] = {"exit", "echo", "type", "pwd", "cd"};
const int BUILTIN_COMMANDS_LENGTH =
    sizeof(BUILTIN_COMMANDS) / sizeof(BUILTIN_COMMANDS[0]);
