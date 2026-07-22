// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_LIB_PATH_H_
#define SRC_LIB_PATH_H_

#include <stddef.h>

char *get_full_path(const char *command);
char *get_home_directory(const char *user);

#endif  // SRC_LIB_PATH_H_
