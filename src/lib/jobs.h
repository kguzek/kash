// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_LIB_JOBS_H_
#define SRC_LIB_JOBS_H_

#include <sys/types.h>

enum JOB_STATUS {
  JOB_STATUS_RUNNING,
  JOB_STATUS_DONE,
};

struct job_definition {
  size_t id;
  pid_t pid;
  const char *cmd;
  enum JOB_STATUS status;
};

static struct job_vec *jobs;
int define_job(pid_t pid, const char *cmd);
struct job_vec **get_job_definitions();

#endif  // SRC_LIB_JOBS_H_
