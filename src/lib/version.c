// Copyright (c) 2026 Konrad Guzek

#include "src/lib/version.h"

#include <stdio.h>
#include <stdlib.h>

#include "src/lib/config.h"

int print_usage() {
  printf(
      "Usage: %s [option]\n\n"
      "Options:\n"
      "   --help      show this help message and exit\n"
      "   --version   show the program version and exit\n"
      "   -c command  run `command` instead of reading from standard input\n",
      PROGRAM_NAME);
  return EXIT_SUCCESS;
}

int print_version() {
  printf("%s version %s\nCopyright (c) 2026 Konrad Guzek\n", PROGRAM_NAME,
         PROGRAM_VERSION);
  return EXIT_SUCCESS;
}
