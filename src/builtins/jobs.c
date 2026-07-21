// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/jobs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/jobs.h"

int builtin_jobs(size_t argc, const char **argv) {
  return print_updated_jobs_list(true);
}
