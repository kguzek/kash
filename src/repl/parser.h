// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_REPL_PARSER_H_
#define SRC_REPL_PARSER_H_

#include <stddef.h>

#include "src/lib/vector.h"

enum COMMAND_SEPARATOR {
  CMD_SEP_NONE,  // Final command in sequence
  CMD_SEP_SQTL,  // Sequential command (;)
  CMD_SEP_PIPE,  // Piped command (|)
  CMD_SEP_BGND,  // Background command (&)
  // TODO(kguzek): add logical operators, e.g. '&&', '||'
};

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
const char ***
allocate_cmdv(size_t cmdc, const size_t argcv[restrict cmdc], char *input,
              enum COMMAND_SEPARATOR cmd_separators[restrict cmdc]);
int handle_redirection(char *input, char *redirection);
static void initialize_cmd_parse_ctx(struct cmd_parse_ctx *ctx);
static const bool is_escapable_in_double_quotes(const char c);
/** Tokenizes the input string starting at `variable_start_offset` bytes after
 * `char_start`, and stops parsing when it encounters a non-variable-name
 * character. Adjusts the value pointed at by `variable_start_offset` if needed,
 * and sets the value at `length_out` to the final length of the variable name,
 * such that `*(char_start + *variable_start_offset)` produces the first
 * character of the variable name, and `*(char_start + *variable_start_offset +
 * *length_out)` is the first character after the variable name.
 * Returns `EXIT_SUCCESS` on success and `EXIT_FAILURE` on failure. */
static int parse_variable_name_length(const char *char_start,
                                      size_t *variable_start_offset,
                                      size_t *length_out);

#endif  // SRC_REPL_PARSER_H_
