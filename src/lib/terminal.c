// Copyright 2026 Konrad Guzek

#include "src/lib/terminal.h"

#include <stdio.h>

int ring_bell() {
  return printf("\x07");
}
