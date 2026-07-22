// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/cd.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/lib/config.h"

int builtin_cd(const size_t argc, const char **argv) {
  const char *command_name = argv[0];
  if (argc > 2) {
    fprintf(stderr, "%s: %s: too many arguments\n", PROGRAM_NAME, command_name);
    return EXIT_FAILURE;
  }
  const char *chdir_target;
  if (argc == 2) {
    chdir_target = argv[1];
  }
  if (argc < 2 || strcmp(chdir_target, "~") == 0) {
    chdir_target = getenv("HOME");
    if (chdir_target == NULL) {
      fprintf(stderr, "%s: %s: failed to get home directory\n", PROGRAM_NAME,
              command_name);
      return 2;
    }
  }
  if (chdir(chdir_target) != 0) {
    // TODO(kguzek): tests require this exact error format
    fprintf(stderr, "%s: %s: %s\n", command_name, chdir_target,
            strerror(errno));
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
