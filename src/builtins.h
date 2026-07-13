// Copyright 2026 Konrad Guzek

#ifndef SRC_BUILTINS_H_
#define SRC_BUILTINS_H_

#include <stddef.h>

int builtin_echo(const size_t argc, const char **argv);
int builtin_type(const size_t argc, const char **argv);
int builtin_pwd(const size_t argc, const char **argv);
int builtin_cd(const size_t argc, const char **argv);

static int get_command_type(const char *input_command);

#endif  // SRC_BUILTINS_H_
