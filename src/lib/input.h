// Copyright 2026 Konrad Guzek

#ifndef SRC_LIB_INPUT_H_
#define SRC_LIB_INPUT_H_

#include <stddef.h>

int process_input(char *input);
size_t collect_input(char **input);
static int calculate_argc(const char *input);
static const char **allocate_argv(size_t args_count, char *input);
static const bool is_escapable_in_double_quotes(const char c);
static int handle_redirection(char *input, char *redirection);

#endif  // SRC_LIB_INPUT_H_
