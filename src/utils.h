// Copyright 2026 Konrad Guzek

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <stddef.h>

#define MAX_PATH_SIZE 1024
#define MAX_INPUT_LINE_LENGTH 1024

size_t collect_input(char **input);
char *get_full_path(const char *command);
int split_string(const char *input, char *output[], int max_output_length);
int run_external_program(char *program_name, const char *program_path,
                         const char *args);
int try_run_external_program(char *first_word, const char *args);
int ring_bell();
char *get_longest_common_prefix(char **programs, size_t nprograms);
int compare_strings(const void *lhs, const void *rhs);

#endif  // SRC_UTILS_H_
