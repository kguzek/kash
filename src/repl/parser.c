// Copyright (c) 2026 Konrad Guzek

#include "src/repl/parser.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/config.h"
#include "src/lib/error_handling.h"
#include "src/lib/path.h"
#include "src/lib/variables.h"
#include "src/lib/vector.h"
#include "src/repl/io.h"

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

#define HANDLE_QUOTES_AND_ESCAPES(ctx, c, label, quotes_label)                 \
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
    goto quotes_label;                                                         \
  case '"':                                                                    \
    if (ctx->in_single_quotes || ctx->next_char_escaped) {                     \
      goto label;                                                              \
    }                                                                          \
    ctx->in_double_quotes = !(ctx->in_double_quotes);                          \
    goto quotes_label;

#define FLUSH_OUTPUT_REDIRECTION_TARGET()                                      \
  struct output_redirection_vec *output_redirections =                         \
      *cmd_output_redirections[cmd_idx];                                       \
  int redirection_idx = output_redirection_vec_size(output_redirections) - 1;  \
  if (redirection_idx < 0) {                                                   \
    print_error(">: unknown redirection");                                     \
    free(cmdv);                                                                \
    return NULL;                                                               \
  }                                                                            \
  struct output_redirection *redirection =                                     \
      output_redirections->value[redirection_idx];                             \
  push_back_char(&current_arg, '\0');                                          \
  redirection->output_target = strdup(current_arg->value);                     \
  free(current_arg);                                                           \
  current_arg = NULL;                                                          \
  ctx->output_redirection_stage = OUT_STAGE_NONE;

#define COPY_PREVIOUS_ARG(ctx, cmdv, cmd_idx, arg_idx, current_arg)            \
  if (ctx->starting_new_arg) {                                                 \
    switch (ctx->output_redirection_stage) {                                   \
    case OUT_STAGE_NONE:                                                       \
      if (arg_idx > 0) {                                                       \
        push_back_char(&current_arg, '\0');                                    \
        cmdv[cmd_idx][arg_idx - 1] = strdup(current_arg->value);               \
        free(current_arg);                                                     \
        current_arg = NULL;                                                    \
      }                                                                        \
      arg_idx++;                                                               \
      break;                                                                   \
    case OUT_STAGE_TARGET_PENDING:                                             \
      ctx->output_redirection_stage = OUT_STAGE_TARGET_PARSED;                 \
      break;                                                                   \
    case OUT_STAGE_TARGET_PARSED:                                              \
      FLUSH_OUTPUT_REDIRECTION_TARGET();                                       \
      break;                                                                   \
    default:                                                                   \
      break;                                                                   \
    }                                                                          \
    ctx->starting_new_arg = false;                                             \
  }                                                                            \
  ctx->starting_new_cmd = false;

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
      HANDLE_QUOTES_AND_ESCAPES(ctx, c, handle_other_char, handle_other_char);
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
  // printf("cmdc=%lu\n", *cmdc);
  // for (size_t i = 0; i < *cmdc; i++) {
  //   printf("cmd %lu argc=%lu\n", i, (*argcv)->value[i]);
  // }
  // printf("---   done calculation\n");
  return EXIT_SUCCESS;
}

char ***allocate_cmdv(size_t *cmdc_ptr, size_t argcv[*cmdc_ptr], char *input,
                      enum COMMAND_SEPARATOR cmd_separators[*cmdc_ptr]) {
  size_t total_args = 0;
  size_t cmdc = *cmdc_ptr;
  for (size_t i = 0; i < cmdc; i++) {
    // TODO(kguzek): add +1 if we use NULL terminators in future
    total_args += argcv[i];
  }
  char ***cmdv = malloc(cmdc * sizeof(*cmdv) + total_args * sizeof(**cmdv));
  if (cmdv == NULL) {
    perror("malloc");
    return NULL;
  }
  struct output_redirection_vec *cmd_output_redirections_a[cmdc];
  struct output_redirection_vec **cmd_output_redirections[cmdc];
  // this sets the first-dimension indices (i.e. cmdv[0..cmdc])
  // as pointers to the appropriate command argv's starting index
  char **argv_storage = (char **)(cmdv + cmdc);
  for (size_t i = 0; i < cmdc; i++) {
    cmd_separators[i] = CMD_SEP_NONE;
    cmd_output_redirections_a[i] = NULL;
    cmd_output_redirections[i] = &cmd_output_redirections_a[i];
    cmdv[i] = argv_storage;
    // TODO(kguzek): add +1 if we use NULL terminators in future
    argv_storage += argcv[i];
  }

  size_t cmd_idx = 0, arg_idx = 0, input_idx = 0;

  struct cmd_parse_ctx ctx_local;
  struct cmd_parse_ctx *ctx = &ctx_local;
  initialize_cmd_parse_ctx(ctx);
  struct char_vec *current_arg = NULL;
  for (const char *c = input; *c != '\0'; c++) {
    const bool char_escaped = ctx->next_char_escaped || ctx->in_single_quotes
                              || ctx->in_double_quotes;
    switch (*c) {
      HANDLE_QUOTES_AND_ESCAPES(ctx, c, copy_char, handle_quotes);
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
      if (ctx->output_redirection_stage == OUT_STAGE_TARGET_PENDING) {
        (*cmdc_ptr)--;
        goto copy_char;
      }
      cmd_separators[cmd_idx] = CMD_SEP_BGND;
      goto separate_command;
    separate_command:
      ctx->starting_new_arg = true;  // to ensure the previous arg is flushed
      COPY_PREVIOUS_ARG(ctx, cmdv, cmd_idx, arg_idx, current_arg);
      argcv[cmd_idx] = arg_idx - 1;
      cmd_idx++;
      arg_idx = 0;
      ctx->starting_new_arg = true;  // COPY_PREVIOUS_ARG resets to false
      ctx->starting_new_cmd = true;
      break;
    case '>':
      if (char_escaped) {
        goto copy_char;
      }
      struct output_redirection *redirection =
          malloc(sizeof(struct output_redirection));
      if (*(c + 1) == '>') {
        redirection->type = OUT_TYPE_APPND;
        c++;
      } else {
        redirection->type = OUT_TYPE_WRITE;
      }
      if (ctx->starting_new_arg) {
        COPY_PREVIOUS_ARG(ctx, cmdv, cmd_idx, arg_idx, current_arg);
        redirection->output_file = "";
      } else {
        if (char_vec_size(current_arg) == 1 && current_arg->value[0] >= '0'
            && current_arg->value[0] <= '9') {
          push_back_char(&current_arg, '\0');
          redirection->output_file = strdup(current_arg->value);
          free(current_arg);
          current_arg = NULL;
        } else {
          ctx->starting_new_arg = true;
          COPY_PREVIOUS_ARG(ctx, cmdv, cmd_idx, arg_idx, current_arg);
          redirection->output_file = "";
        }
      }
      arg_idx--;
      push_back_output_redirection(cmd_output_redirections[cmd_idx],
                                   redirection);
      ctx->output_redirection_stage = OUT_STAGE_TARGET_PENDING;
      ctx->starting_new_arg = true;
      break;
    case '~':
      if (char_escaped) {
        goto copy_char;
      }
      char *home_directory = get_home_directory(NULL);
      COPY_PREVIOUS_ARG(ctx, cmdv, cmd_idx, arg_idx, current_arg);
      size_t home_dir_size = 0;
      for (char *h = home_directory; *h != '\0'; h++) {
        push_back_char(&current_arg, *h);
        home_dir_size++;
      }
      break;
    case '$':
      if (ctx->in_single_quotes || ctx->next_char_escaped) {
        goto copy_char;
      }
      size_t variable_name_length = 0, variable_start_offset = 1,
             variable_end_offset = 0;
      const int result = parse_variable_name_length(c, &variable_start_offset,
                                                    &variable_end_offset,
                                                    &variable_name_length);
      if (result != EXIT_SUCCESS) {
        free(cmdv);
        return NULL;
      }
      if (variable_name_length == 0 && variable_end_offset == 0) {
        // matches e.g. "$.", "$/" but not "${}"
        goto copy_char;
      }
      {
        char variable_name[variable_name_length + 1];
        memcpy(variable_name, c + variable_start_offset, variable_name_length);
        variable_name[variable_name_length] = '\0';
        char *variable_value = get_variable_value(variable_name);
        if (strlen(variable_value) > 0) {
          COPY_PREVIOUS_ARG(ctx, cmdv, cmd_idx, arg_idx, current_arg);
        }
        for (char *v = variable_value; *v != '\0'; v++) {
          push_back_char(&current_arg, *v);
        }
        c += variable_start_offset + variable_name_length + variable_end_offset
             - 1;
      }
      break;
    case ' ':
      if (char_escaped) {
        goto copy_char;
      }
      ctx->starting_new_arg = true;
      break;
    handle_quotes:
      COPY_PREVIOUS_ARG(ctx, cmdv, cmd_idx, arg_idx, current_arg)
      break;
    default:
    copy_char:
      COPY_PREVIOUS_ARG(ctx, cmdv, cmd_idx, arg_idx, current_arg)
      ctx->next_char_escaped = false;
      push_back_char(&current_arg, *c);
      break;
    }
  }
  // ensure final arg is also NULL-terminated
  if (!ctx->starting_new_cmd) {
    push_back_char(&current_arg, '\0');
    argcv[cmd_idx] = arg_idx;
  }
  if (ctx->output_redirection_stage == OUT_STAGE_TARGET_PENDING) {
    print_error(">: missing redirection target");
    free(cmdv);
    return NULL;
  }
  if (char_vec_size(current_arg) > 0) {
    char *current_arg_value = strdup(current_arg->value);
    if (ctx->output_redirection_stage == OUT_STAGE_TARGET_PARSED) {
      FLUSH_OUTPUT_REDIRECTION_TARGET();
    } else if (arg_idx == 0) {
      cmdv[cmd_idx - 1][arg_idx] = current_arg_value;
    } else {
      cmdv[cmd_idx][arg_idx - 1] = current_arg_value;
    }
  }
  if (current_arg != NULL) {
    free(current_arg);
  }
  // for (size_t cmdi = 0; cmdi < cmdc; cmdi++) {
  //   for (size_t argi = 0; argi < argcv[cmdi]; argi++) {
  //     printf("%s ", cmdv[cmdi][argi]);
  //   }
  //   enum COMMAND_SEPARATOR sep = cmd_separators[cmdi];
  //   printf("(%s) \n", sep == CMD_SEP_PIPE   ? "pipe"
  //                     : sep == CMD_SEP_BGND ? "bgnd"
  //                     : sep == CMD_SEP_SQTL ? "sqtl"
  //                     : sep == CMD_SEP_NONE ? "none"
  //                                           : "unknown");
  // }

  // for (size_t i = 0; i < cmdc; i++) {
  //   printf("cmd %lu argc=%lu\n", i, argcv[i]);
  // }
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
  const char *redirect_repr = file_mode[0] == 'a' ? ">>" : ">";
  if (*redirection == '\0') {
    fprintf(stderr, "%s: %s: missing redirection target\n", PROGRAM_NAME,
            redirect_repr);
    return EXIT_FAILURE;
  }
  FILE *file = freopen(redirection, file_mode, output_file);
  if (file == NULL) {
    fprintf(stderr, "%s: %s%s: %s\n", PROGRAM_NAME, redirect_repr, redirection,
            strerror(errno));
    reset_output();
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static void initialize_cmd_parse_ctx(struct cmd_parse_ctx *ctx) {
  ctx->starting_new_arg = true;
  ctx->starting_new_cmd = true;
  ctx->in_single_quotes = false;
  ctx->in_double_quotes = false;
  ctx->next_char_escaped = false;
  ctx->output_redirection_stage = OUT_STAGE_NONE;
}

static const bool is_escapable_in_double_quotes(const char c) {
  return c == '\\' || c == '"';
}

static int parse_variable_name_length(const char *char_start,
                                      size_t *variable_start_offset,
                                      size_t *variable_end_offset,
                                      size_t *length_out) {
  const char *char_end = char_start + *variable_start_offset;
  bool seen_brace = false;
  while (*char_end != '\0') {
    bool is_first_char = char_end == char_start + *variable_start_offset;
    if (*char_end == '?') {
      if (!is_first_char) {
        break;
      }
      if (!seen_brace) {
        char_end++;
        break;
      }
      if (*(++char_end) == '}') {
        continue;
      }
      fprintf(stderr, "%s: invalid substitution value\n", PROGRAM_NAME);
      return EXIT_FAILURE;
    } else if (*char_end == '{') {
      if (seen_brace) {
        fprintf(stderr, "%s: %c: unexpected nested brace in substitution\n",
                PROGRAM_NAME, *char_end);
        return EXIT_FAILURE;
      }
      if (is_first_char) {
        // starts parsing "${foo}" as variable "$foo"
        seen_brace = true;
        (*variable_start_offset)++;
      } else {
        // parses "$foo{bar}" as variable "$foo" and literal "{bar}"
        break;
      }
    } else if (*char_end == '}') {
      if (seen_brace) {
        (*variable_end_offset)++;
      } else {
        // fprintf(stderr, "%s: %c: unmatched closing brace in substitution\n",
        //         PROGRAM_NAME, *char_end);
      }
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
