// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_BUILTINS_HISTORY_H_
#define SRC_BUILTINS_HISTORY_H_

#include <stddef.h>

int builtin_history(size_t argc, const char *argv[argc]);
static int print_history_list(long *last_n_entries);

#endif  // SRC_BUILTINS_HISTORY_H_
