// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_REPL_IO_H_
#define SRC_REPL_IO_H_

#include <stddef.h>

size_t collect_input(char **input);
int reset_output();
int reset_input();

#endif  // SRC_REPL_IO_H_
