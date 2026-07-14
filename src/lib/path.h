// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_LIB_PATH_H_
#define SRC_LIB_PATH_H_

#include <stddef.h>

char *get_full_path(const char *command);
int try_run_external_program(const size_t argc, const char **argv);
static int run_external_program(const size_t argc, const char **argv,
                                const char *program_path);

#endif  // SRC_LIB_PATH_H_
