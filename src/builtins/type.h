// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_BUILTINS_TYPE_H_
#define SRC_BUILTINS_TYPE_H_

#include <stddef.h>

int builtin_type(const size_t argc, const char **argv);
static int get_command_type(const char *input_command);

#endif  // SRC_BUILTINS_TYPE_H_
