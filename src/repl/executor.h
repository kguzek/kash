// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_REPL_EXECUTOR_H_
#define SRC_REPL_EXECUTOR_H_

#include <stddef.h>
#include <sys/types.h>

#include "src/lib/vector.h"
#include "src/repl/parser.h"

int execute_commands(size_t cmdc, const char **cmdv[cmdc],
                     const enum COMMAND_SEPARATOR cmd_separators[cmdc],
                     const size_t argcv[cmdc]);
int run_external_program(const size_t argc, const char *argv[argc],
                         const char *program_path, pid_t *pid_ptr,
                         const struct string_pair_vec *additional_envs);
static int execute_command(const size_t argc, const char *argv[argc],
                           pid_t *pid_ptr);
static int try_run_external_program(const size_t argc, const char *argv[argc],
                                    pid_t *pid_ptr);

#endif  // SRC_REPL_EXECUTOR_H_
