// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_LIB_CONFIG_H_
#define SRC_LIB_CONFIG_H_

extern const char *PROGRAM_NAME;
extern const char *BUILTIN_COMMANDS[];
extern const int BUILTIN_COMMANDS_LENGTH;

enum {
  MAX_PATH_SIZE = 1024,
  MAX_INPUT_LINE_LENGTH = 1024,
};

int get_previous_exit_code();
int set_previous_exit_code(int new_exit_code);

#endif  // SRC_LIB_CONFIG_H_
