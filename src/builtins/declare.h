// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_BUILTINS_DECLARE_H_
#define SRC_BUILTINS_DECLARE_H_

#include <stddef.h>

int builtin_declare(size_t argc, const char *argv[argc]);
static int print_declared_variable(const char *command_name,
                                   const char *variable);

#endif  // SRC_BUILTINS_DECLARE_H_
