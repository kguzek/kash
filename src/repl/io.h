// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_REPL_IO_H_
#define SRC_REPL_IO_H_

#include <stddef.h>

int prepare_input();
size_t collect_input(char **input);
int interrupt_input();
int reset_output();

#endif  // SRC_REPL_IO_H_
