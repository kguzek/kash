// Copyright 2026 Konrad Guzek

#ifndef SRC_REPL_COMPLETIONS_H_
#define SRC_REPL_COMPLETIONS_H_

#include <stddef.h>

#include "src/lib/vector.h"

int autocomplete(int count, int key);
static int autocomplete_commands(const char *cmd);
static int autocomplete_arguments(const size_t argc, const char **argv);
static int autocomplete_builtins(const char *cmd);
static int autocomplete_externals(const char *cmd);
static int autocomplete_filenames(const char *current_token);

/** Depending on the number of available completions in this context, either
 * prints the list of completions  or inserts an unambiguous completion
 * (`completions[0]`) to the input buffer.
 * @returns `EXIT_FAILURE` if the bell should be rung, or `EXIT_SUCCESS` if a
 * single completion was selected and inserted. */
static int insert_completions(struct string_vec *completions,
                              const char *current_token);
/** Populates `programs` with the names of all executable files in the
 * directories listed in the PATH environment variable, such that their name
 * begins with `current_token`. */
static int get_matching_externals(struct string_vec **externals,
                                  const char *current_token);
/* Populates `filenames` with the names of all regular files, symlinks or
 * directories in the current working directory, such that their name begins
 * with `current_token`. If `current_token` contains a path, then the directory
 * part is used for searching. Otherwise, the current working directory is used.
 */
static int get_matching_filenames(struct string_vec **filenames,
                                  const char *current_token);

#endif  // SRC_REPL_COMPLETIONS_H_
