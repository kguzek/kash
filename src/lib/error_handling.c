// Copyright (c) 2026 Konrad Guzek

#include "src/lib/error_handling.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/config.h"

#define PRINT_ERROR(fmt)                                                       \
  int result = fprintf(stderr, "%s: ", PROGRAM_NAME);                          \
  va_list args;                                                                \
  va_start(args, fmt);                                                         \
  result += vfprintf(stderr, fmt, args);                                       \
  va_end(args);

int print_error(const char *fmt, ...) {
  PRINT_ERROR(fmt);
  return result + fprintf(stderr, "\n");
}

int print_error_syscall(const char *fmt, ...) {
  int error = errno;
  PRINT_ERROR(fmt);
  return result + fprintf(stderr, ": %s\n", strerror(error));
}
