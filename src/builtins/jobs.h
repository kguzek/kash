// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_BUILTINS_JOBS_H_
#define SRC_BUILTINS_JOBS_H_

#include <stddef.h>

#include "src/lib/jobs.h"

int builtin_jobs(size_t argc, const char **argv);
static int update_job_status(struct job_definition *job);
static char *get_job_status_text(enum JOB_STATUS status);

#endif  // SRC_BUILTINS_JOBS_H_
