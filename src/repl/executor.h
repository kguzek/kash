// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_REPL_EXECUTOR_H_
#define SRC_REPL_EXECUTOR_H_

#include <stddef.h>
#include <sys/types.h>

int execute_commands(size_t cmdc, const char ***cmdv, const bool *cmd_pipes,
                     const size_t argcv[]);
static int execute_command(const size_t argc, const char **argv,
                           pid_t *pid_ptr);
static int try_run_external_program(const size_t argc, const char **argv,
                                    pid_t *pid_ptr);
static int run_external_program(const size_t argc, const char **argv,
                                const char *program_path, pid_t *pid_ptr);

#endif  // SRC_REPL_EXECUTOR_H_
