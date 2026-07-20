// Copyright (c) 2026 Konrad Guzek

#include "src/lib/jobs.h"

#include <stdio.h>
#include <stdlib.h>

#include "src/lib/vector.h"

static struct job_vec *jobs = NULL;

int define_job(pid_t pid, const char *cmd) {
  struct job_definition *job = malloc(sizeof(struct job_definition));
  size_t jobs_size = job_vec_size(jobs);
  job->id = jobs_size + 1;
  job->pid = pid;
  job->cmd = cmd;
  job->status = JOB_STATUS_RUNNING;
  printf("[%lu] %d\n", job->id, pid);
  return push_back_job(&jobs, job);
}

struct job_vec **get_job_definitions() {
  return &jobs;
}
