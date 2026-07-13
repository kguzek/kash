// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/pwd.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int builtin_pwd(const size_t argc, const char **argv) {
  char *cwd = getcwd(NULL, 0);
  if (cwd == NULL) {
    perror(argv[0]);
    return EXIT_FAILURE;
  }
  printf("%s\n", cwd);
  free(cwd);
  return EXIT_SUCCESS;
}
