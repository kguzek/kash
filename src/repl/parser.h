// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_REPL_PARSER_H_
#define SRC_REPL_PARSER_H_

#include <stddef.h>

#include "src/lib/vector.h"

struct cmd_parse_ctx {
  bool in_single_quotes;
  bool in_double_quotes;
  bool next_char_escaped;
  bool starting_new_arg;
  bool starting_new_cmd;
};

/** Calculates the number of commands in the input string and the number of
 * arguments for each command. The input string is not modified. The function
 * returns EXIT_SUCCESS on success, or EXIT_FAILURE on failure. If `ctx_out` is
 * not `NULL`, uses it to store final state, such as if the input is starting a
 * new argument or command, or if there are unmatched quotes; the function will
 * ignore syntax errors and return the number of commands and arguments as best
 * as it can. If it is `NULL`, the function will return `EXIT_FAILURE` if there
 * are any syntax errors in the input string (e.g., unclosed quotes, missing
 * command after a semicolon or pipe) */
int calculate_cmdc(const char *input, size_t *cmdc, struct size_t_vec **argv,
                   struct cmd_parse_ctx *ctx_out);
const char ***allocate_cmdv(size_t cmdc, const size_t argcv[], char *input,
                            bool *cmd_pipes);
int handle_redirection(char *input, char *redirection);
static void initialize_cmd_parse_ctx(struct cmd_parse_ctx *ctx);
static const bool is_escapable_in_double_quotes(const char c);

#endif  // SRC_REPL_PARSER_H_
