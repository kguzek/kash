// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/cd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int builtin_cd(const size_t argc, const char **argv) {
  if (argc > 2) {
    fprintf(stderr, "%s: too many arguments\n", argv[0]);
    return EXIT_FAILURE;
  }
  const char *chdir_target;
  if (argc == 2) {
    chdir_target = argv[1];
  }
  if (argc < 2 || strcmp(chdir_target, "~") == 0) {
    chdir_target = getenv("HOME");
    if (chdir_target == NULL) {
      fprintf(stderr, "%s: failed to get home directory\n", argv[0]);
      return 2;
    }
  }
  if (chdir(chdir_target) != 0) {
    perror(argv[0]);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
