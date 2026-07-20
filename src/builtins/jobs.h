// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_BUILTINS_JOBS_H_
#define SRC_BUILTINS_JOBS_H_

#include <stddef.h>

#include "src/lib/jobs.h"

int builtin_jobs(size_t argc, const char **argv);
static char *get_job_status_text(enum JOB_STATUS status);

#endif  // SRC_BUILTINS_JOBS_H_
