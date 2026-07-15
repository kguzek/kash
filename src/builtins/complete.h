// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_BUILTINS_COMPLETE_H_
#define SRC_BUILTINS_COMPLETE_H_

#include <stddef.h>

int builtin_complete(const size_t argc, const char **argv);
static int print_registration_spec(const char *program_name, const char *cmd);

#endif  // SRC_BUILTINS_COMPLETE_H_
