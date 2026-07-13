// Copyright 2026 Konrad Guzek

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <stddef.h>

size_t collect_input(char **input);
char *get_full_path(const char *command);
int split_string(const char *input, char *output[], int max_output_length);
int run_external_program(char *program_name, const char *program_path,
                         const char *args);
int try_run_external_program(char *first_word, const char *args);
int ring_bell();

#endif  // SRC_UTILS_H_
