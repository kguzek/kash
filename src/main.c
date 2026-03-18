// Copyright 2026 Konrad Guzek

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void prompt(char **input, size_t *size) {
  *size = 0;

  printf("$ ");

  getline(input, size, stdin);

  // remove trailing newline
  size_t len = strlen(*input);
  if (len > 0 && (*input)[len - 1] == '\n') {
    (*input)[len - 1] = '\0';
  }

  printf("%s: command not found\n", *input);
}

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  char *input = NULL;
  size_t size = 0;
  while (1) {
    prompt(&input, &size);
  }

  free(input);
  return 1;
}
