// Copyright 2026 Konrad Guzek

#include <stdio.h>

#include "src/repl/shell.h"

int main(int argc, char *argv[]) {
  // flush after every printf
  setbuf(stdout, NULL);
  return loop();
}
