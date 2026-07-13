// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/echo.h"

#include <stdio.h>
#include <stdlib.h>

int builtin_echo(const size_t argc, const char **argv) {
  if (argc > 1) {
    printf("%s", argv[1]);
    for (size_t i = 2; i < argc; i++) {
      printf(" %s", argv[i]);
    }
  }
  printf("\n");
  return EXIT_SUCCESS;
}
