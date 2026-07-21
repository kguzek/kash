// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_LIB_HISTORY_H_
#define SRC_LIB_HISTORY_H_

#include <stddef.h>
#include <stdio.h>
#include <readline/history.h>

HIST_ENTRY **get_history_entries();
size_t get_history_size();
int push_history_entry(const char *cmd);
int read_history_from_file(const char *path);

#endif  // SRC_LIB_HISTORY_H_
