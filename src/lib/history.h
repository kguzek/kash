// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_LIB_HISTORY_H_
#define SRC_LIB_HISTORY_H_

static struct string_vec *history;

struct string_vec *get_history_entries();
int push_history_entry(const char *cmd);

#endif  // SRC_LIB_HISTORY_H_
