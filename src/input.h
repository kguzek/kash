// Copyright 2026 Konrad Guzek

#ifndef SRC_INPUT_H_
#define SRC_INPUT_H_

#include <stddef.h>

int process_input(char *input);
size_t collect_input(char **input);
static size_t calculate_args_count(const char *input);
static const char **allocate_args_array(size_t args_count, char *input);
static int parse_input(char *input, char *output[]);
static int handle_redirection(char *input, char *redirection);

#endif  // SRC_INPUT_H_
