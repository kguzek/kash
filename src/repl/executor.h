// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_REPL_EXECUTOR_H_
#define SRC_REPL_EXECUTOR_H_

#include <stddef.h>

int execute_commands(size_t cmdc, const char ***cmdv, const size_t argcv[]);
static int execute_command(const size_t argc, const char **argv);
static int try_run_external_program(const size_t argc, const char **argv);
static int run_external_program(const size_t argc, const char **argv,
                                const char *program_path);

#endif  // SRC_REPL_EXECUTOR_H_
