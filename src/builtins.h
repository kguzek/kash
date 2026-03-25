// Copyright 2026 Konrad Guzek

#ifndef SRC_BUILTINS_H_
#define SRC_BUILTINS_H_

int builtin_echo(const char *args);
int builtin_type(const char *args);
int builtin_pwd(const char *args);
int builtin_cd(char *args);

#endif  // SRC_BUILTINS_H_
