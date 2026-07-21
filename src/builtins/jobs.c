// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/jobs.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "src/lib/config.h"
#include "src/lib/jobs.h"
#include "src/lib/vector.h"

static const size_t STATUS_MAX_WIDTH = 24;

int builtin_jobs(size_t argc, const char **argv) {
  struct job_vec *jobs = *get_job_definitions();
  size_t jobs_size = job_vec_size(jobs);
  struct job_definition *job;
  size_t last_job_id = 0, penultimate_job_id = 0;
  for (size_t i = jobs_size; i > 0; i--) {
    job = jobs->value[i - 1];
    if (job->status == JOB_STATUS_DONE) {
      continue;
    }
    if (last_job_id == 0) {
      last_job_id = job->id;
      continue;
    }
    penultimate_job_id = job->id;
    break;
  }
  for (size_t i = 0; i < jobs_size; i++) {
    job = jobs->value[i];
    if (job->status == JOB_STATUS_DONE) {
      continue;
    }
    char job_indicator = job->id == penultimate_job_id ? '-'
                         : job->id == last_job_id      ? '+'
                                                       : ' ';
    update_job_status(job);
    char *job_status = get_job_status_text(job->status);
    char status_padding[STATUS_MAX_WIDTH];
    float job_id_width = log10f(job->id * 10);
    // 5 chars used by syntax: "[]", `job_indicator`, and the two spaces after
    size_t used_chars = 5 + (size_t)job_id_width + strlen(job_status);
    size_t padding_chars =
        used_chars > STATUS_MAX_WIDTH ? 0 : STATUS_MAX_WIDTH - used_chars;
    for (size_t i = 0; i < padding_chars; i++) {
      status_padding[i] = ' ';
    }
    status_padding[padding_chars] = '\0';
    printf("[%lu]%c  %s%s%s\n", job->id, job_indicator, job_status,
           status_padding, job->cmd);
  }
  return EXIT_SUCCESS;
}

static int update_job_status(struct job_definition *job) {
  if (job->status != JOB_STATUS_RUNNING) {
    return EXIT_SUCCESS;
  }
  int status;
  pid_t result = waitpid(job->pid, &status, WNOHANG);
  if (result == job->pid && WIFEXITED(status)) {
    job->status = JOB_STATUS_DONE;
  }
  return EXIT_SUCCESS;
}

static char *get_job_status_text(enum JOB_STATUS status) {
  switch (status) {
  case JOB_STATUS_RUNNING:
    return "Running";
  case JOB_STATUS_DONE:
    return "Done";
  default:
    fprintf(stderr, "%s: unknown job status: %d\n", PROGRAM_NAME, status);
    return "Unknown status";
  }
}
