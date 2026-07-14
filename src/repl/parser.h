// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_REPL_PARSER_H_
#define SRC_REPL_PARSER_H_

#include <stddef.h>

#include "src/lib/vector.h"

int calculate_cmdc(const char *input, size_t *cmdc, struct size_t_vec **argv);
const char ***allocate_cmdv(size_t cmdc, const size_t argcv[], char *input);
int handle_redirection(char *input, char *redirection);
static const bool is_escapable_in_double_quotes(const char c);

#endif  // SRC_REPL_PARSER_H_
