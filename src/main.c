// Copyright 2026 Konrad Guzek

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/error_handling.h"
#include "src/lib/version.h"
#include "src/repl/shell.h"

int main(int argc, char *argv[]) {
  // flush after every printf
  setbuf(stdout, NULL);
  if (argc < 2) {
    return loop();
  }
  const char *arg;
  arg = argv[1];
  if (strcmp(arg, "--help") == 0) {
    return print_usage();
  }
  if (strcmp(arg, "--version") == 0) {
    return print_version();
  }
  if (strcmp(arg, "-c") == 0) {
    if (argc < 3) {
      print_error("%s: missing option argument", arg);
      return EXIT_FAILURE;
    }
    return process_input(argv[2]);
  }
  print_error("unexpected argument: %s", argv[1]);
  return EXIT_FAILURE;
}
