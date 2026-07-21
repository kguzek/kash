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
int print_updated_jobs_list(bool print_all_jobs);
static struct job_vec **get_job_definitions();
static size_t get_new_job_id();
static int update_job_status(struct job_definition *job);
static char *get_job_status_text(enum JOB_STATUS status);

#endif  // SRC_LIB_JOBS_H_
