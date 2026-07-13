// Copyright 2026 Konrad Guzek

#ifndef SRC_COMPLETIONS_H_
#define SRC_COMPLETIONS_H_

#include <stddef.h>

#define EXIT_FAILURE_EXHAUSTIVE 10
#define MAX_INPUT_LINE_LENGTH 1024

int autocomplete(int count, int key);
int autocomplete_builtins();
int autocomplete_external_programs();
int autocomplete_arguments();
int autocomplete_filenames(const char *args);

static int autocomplete_values(char *values[], int nvalues,
                               const char *current_token);
/** Populates `programs` with the names of all executable files in the
 * directories listed in the PATH environment variable, such that their name
 * begins with `rl_line_buffer`. */
static int get_external_programs(char *programs[], int *nprograms);
/* Populates `filenames` with the names of all regular files, symlinks or
 * directories in the current working directory, such that their name begins
 * with `prefix`. If `prefix` contains a path, then the directory part is used
 * for searching. Otherwise, the current working directory is used. */
static int get_matching_filenames(const char *prefix, char **filenames,
                                  int *nfilenames);

#endif  // SRC_COMPLETIONS_H_
