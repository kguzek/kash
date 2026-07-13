// Copyright 2026 Konrad Guzek

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/input.h"

int main(int argc, char *argv[]) {
  // flush after every printf
  setbuf(stdout, NULL);

  char *input = NULL;
  collect_input(&input);
  while (input != NULL && strncmp(input, "exit", 4) != 0) {
    process_input(input);
    collect_input(&input);
  }

  free(input);
  return 0;
}
