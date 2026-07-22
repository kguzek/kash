// Copyright (c) 2026 Konrad Guzek

#include "src/repl/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/config.h"
#include "src/lib/variables.h"
#include "src/lib/vector.h"

static bool is_backslash_escaped(struct cmd_parse_ctx *ctx, const char *c) {
  if (ctx->in_single_quotes || ctx->next_char_escaped) {
    return true;
  }
  if (!ctx->in_double_quotes) {
    return false;
  }
  const char next_char = *(c + 1);
  return !is_escapable_in_double_quotes(next_char);
}

#define HANDLE_QUOTES_AND_ESCAPES(ctx, c, label)                               \
  case '\\':                                                                   \
    if (is_backslash_escaped(ctx, c)) {                                        \
      goto label;                                                              \
    }                                                                          \
    ctx->next_char_escaped = true;                                             \
    break;                                                                     \
  case '\'':                                                                   \
    if (ctx->in_double_quotes || ctx->next_char_escaped) {                     \
      goto label;                                                              \
    }                                                                          \
    ctx->in_single_quotes = !(ctx->in_single_quotes);                          \
    break;                                                                     \
  case '"':                                                                    \
    if (ctx->in_single_quotes || ctx->next_char_escaped) {                     \
      goto label;                                                              \
    }                                                                          \
    ctx->in_double_quotes = !(ctx->in_double_quotes);                          \
    break;

int calculate_cmdc(const char *input, size_t *cmdc, struct size_t_vec **argcv,
                   struct cmd_parse_ctx *ctx_out) {
  const bool strict = ctx_out == NULL;
  struct cmd_parse_ctx ctx_local;
  struct cmd_parse_ctx *ctx = strict ? &ctx_local : ctx_out;
  initialize_cmd_parse_ctx(ctx);
  char last_command_separator = ' ';
  for (const char *c = input; *c != '\0'; c++) {
    const bool char_escaped = ctx->next_char_escaped || ctx->in_single_quotes
                              || ctx->in_double_quotes;
    switch (*c) {
      HANDLE_QUOTES_AND_ESCAPES(ctx, c, handle_other_char);
    case ';':
      if (char_escaped) {
        goto handle_other_char;
      }
      if (ctx->starting_new_cmd) {
        if (strict) {
          fprintf(stderr, "%s: ;: missing command\n", PROGRAM_NAME);
        }
        return EXIT_FAILURE;
      }
      last_command_separator = ';';
      goto separate_command;
    case '|':
      if (char_escaped) {
        goto handle_other_char;
      }
      if (ctx->starting_new_cmd) {
        if (strict) {
          fprintf(stderr, "%s: |: missing pipe source\n", PROGRAM_NAME);
        }
        return EXIT_FAILURE;
      }
      last_command_separator = '|';
      goto separate_command;
    case '&':
      if (char_escaped) {
        goto handle_other_char;
      }
      if (ctx->starting_new_cmd) {
        if (strict) {
          fprintf(stderr, "%s: &: missing background job command\n",
                  PROGRAM_NAME);
        }
        return EXIT_FAILURE;
      }
      last_command_separator = '&';
      goto separate_command;
    separate_command:
      ctx->starting_new_cmd = true;
      ctx->starting_new_arg = true;
      break;
    case ' ':
      if (char_escaped) {
        goto handle_other_char;
      }
      ctx->starting_new_arg = true;
      break;
    default:
    handle_other_char:
      if (ctx->starting_new_cmd) {
        (*cmdc)++;
        push_back_size_t(argcv, 0);
        ctx->starting_new_cmd = false;
      }
      if (ctx->starting_new_arg) {
        (*argcv)->value[*cmdc - 1]++;
        ctx->starting_new_arg = false;
      }
      ctx->next_char_escaped = false;
      break;
    }
  }
  if ((*cmdc) > 0 && ctx->starting_new_cmd && last_command_separator == '|') {
    if (strict) {
      fprintf(stderr, "%s: %c: missing pipe target\n", PROGRAM_NAME,
              last_command_separator);
      return EXIT_FAILURE;
    }
    (*cmdc)++;
  }
  if (strict) {
    if (ctx->in_single_quotes) {
      fprintf(stderr, "%s: ': unmatched single quote\n", PROGRAM_NAME);
      return EXIT_FAILURE;
    }
    if (ctx->in_double_quotes) {
      fprintf(stderr, "%s: \": unmatched double quote\n", PROGRAM_NAME);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

const char ***
allocate_cmdv(size_t cmdc, const size_t argcv[restrict cmdc], char *input,
              enum COMMAND_SEPARATOR cmd_separators[restrict cmdc]) {
  size_t total_args = 0;
  for (size_t i = 0; i < cmdc; i++) {
    // TODO(kguzek): add +1 if we use NULL terminators in future
    total_args += argcv[i];
  }
  const char ***cmdv =
      malloc(cmdc * sizeof(*cmdv) + total_args * sizeof(**cmdv));
  if (cmdv == NULL) {
    perror("malloc");
    return NULL;
  }
  // this sets the first-dimension indices (i.e. cmdv[0..cmdc])
  // as pointers to the appropriate command argv's starting index
  const char **argv_storage = (const char **)(cmdv + cmdc);
  for (size_t i = 0; i < cmdc; i++) {
    cmd_separators[i] = CMD_SEP_NONE;
    cmdv[i] = argv_storage;
    // TODO(kguzek): add +1 if we use NULL terminators in future
    argv_storage += argcv[i];
  }

  const char *input_copy = strdup(input);
  size_t cmd_idx = 0, arg_idx = 0, input_idx = 0;

  struct cmd_parse_ctx ctx_local;
  struct cmd_parse_ctx *ctx = &ctx_local;
  initialize_cmd_parse_ctx(ctx);
  struct char_vec *current_arg = NULL;
  for (const char *c = input_copy; *c != '\0'; c++) {
    const bool char_escaped = ctx->next_char_escaped || ctx->in_single_quotes
                              || ctx->in_double_quotes;
    switch (*c) {
      HANDLE_QUOTES_AND_ESCAPES(ctx, c, copy_char);
    case ';':
      if (char_escaped) {
        goto copy_char;
      }
      cmd_separators[cmd_idx] = CMD_SEP_SQTL;
      goto separate_command;
    case '|':
      if (char_escaped) {
        goto copy_char;
      }
      cmd_separators[cmd_idx] = CMD_SEP_PIPE;
      goto separate_command;
    case '&':
      if (char_escaped) {
        goto copy_char;
      }
      cmd_separators[cmd_idx] = CMD_SEP_BGND;
      goto separate_command;
    separate_command:
      // NULL-terminate last arg of previous command
      push_back_char(&current_arg, '\0');
      cmd_idx++;
      arg_idx = 0;
      ctx->starting_new_arg = true;
      ctx->starting_new_cmd = true;
      break;
    case '$':
      if (char_escaped) {
        goto copy_char;
      }
      size_t variable_name_length = 0, variable_start_offset = 1;
      const int result = parse_variable_name_length(c, &variable_start_offset,
                                                    &variable_name_length);
      if (result != EXIT_SUCCESS) {
        free(cmdv);
        return NULL;
      }
      {
        char variable_name[variable_name_length + 1];
        memcpy(variable_name, c + variable_start_offset, variable_name_length);
        variable_name[variable_name_length] = '\0';
        char *variable_value = get_variable_value(variable_name);
        // for (char *v = variable_value; *v != '\0'; v++) {
        //   push_back_char(&current_arg, *v);
        // }
        // printf("parsed variable '%s': '%s'\n", variable_name,
        // variable_value);
      }
      goto copy_char;
    case ' ':
      if (char_escaped) {
        goto copy_char;
      }
      if (ctx->starting_new_arg) {
        break;  // repeated unquoted space: no-op
      }
      // first unquoted space
      ctx->starting_new_arg = true;
      break;
    default:
    copy_char:
      if (ctx->starting_new_arg) {
        if (cmd_idx > 0 || arg_idx > 0) {
          push_back_char(&current_arg, '\0');
          cmdv[cmd_idx][arg_idx - 1] = strdup(current_arg->value);
          free(current_arg);
          current_arg = NULL;
        }
        ctx->starting_new_arg = false;
        arg_idx++;
      }
      push_back_char(&current_arg, *c);
      ctx->next_char_escaped = false;
      break;
    }
  }
  // ensure final arg is also NULL-terminated
  push_back_char(&current_arg, '\0');
  cmdv[cmd_idx][arg_idx - 1] = strdup(current_arg->value);
  if (current_arg != NULL) {
    free(current_arg);
  }
  return cmdv;
}

int handle_redirection(char *input, char *redirection) {
  if (redirection == NULL) {
    return EXIT_SUCCESS;
  }
  char *fd_input = redirection;
  if (redirection == input) {
    fprintf(stderr, "Using output redirection at the beginning of the command "
                    "is not implemented\n");
    return EXIT_FAILURE;
  }
  char fd_number = *(fd_input - 1);
  FILE *output_file = stdout;
  char *file_mode = "w";
  if (redirection != input) {
    if (fd_number == '1') {
      fd_input--;
    } else if (fd_number == '2') {
      fd_input--;
      output_file = stderr;
    }
  }
  *fd_input = '\0';
  // input_length = fd_input - input;
  redirection++;
  if (redirection != NULL && *redirection == '>') {
    file_mode = "a";
    redirection++;
  }
  while (*redirection == ' ') {
    redirection++;
  }
  if (*redirection == '\0') {
    fprintf(stderr, "No file specified for redirection\n");
    return EXIT_FAILURE;
  }
  freopen(redirection, file_mode, output_file);
  return EXIT_SUCCESS;
}

static void initialize_cmd_parse_ctx(struct cmd_parse_ctx *ctx) {
  ctx->starting_new_arg = true;
  ctx->starting_new_cmd = true;
  ctx->in_single_quotes = false;
  ctx->in_double_quotes = false;
  ctx->next_char_escaped = false;
}

static const bool is_escapable_in_double_quotes(const char c) {
  return c == '\\' || c == '"';
}

static int parse_variable_name_length(const char *char_start,
                                      size_t *variable_start_offset,
                                      size_t *length_out) {
  const char *char_end = char_start + *variable_start_offset;
  bool seen_brace = false;
  while (*char_end != '\0') {
    if (*char_end == '{') {
      if (seen_brace) {
        fprintf(stderr, "%s: %c: unexpected nested brace in substitution\n",
                PROGRAM_NAME, *char_end);
        return EXIT_FAILURE;
      }
      if (char_end == char_start + *variable_start_offset) {
        // starts parsing "${foo}" as variable "$foo"
        seen_brace = true;
        (*variable_start_offset)++;
      } else {
        // parses "$foo{bar}" as variable "$foo" and literal "{bar}"
        break;
      }
    } else if (*char_end == '}') {
      // if (!seen_brace) {
      //   fprintf(stderr, "%s: %c: unmatched closing brace in substitution\n",
      //           PROGRAM_NAME, *cc);
      // }
      seen_brace = false;
      break;
    } else if (!is_valid_variable_char(*char_end)) {
      break;
    }
    char_end++;
  }
  if (seen_brace) {
    fprintf(stderr, "%s: {: unmatched opening brace in substitution\n",
            PROGRAM_NAME);
    return EXIT_FAILURE;
  }
  *length_out = char_end - char_start - *variable_start_offset;
  return EXIT_SUCCESS;
}
