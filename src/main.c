// Copyright 2026 Konrad Guzek

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void collect_input(char **input, size_t *size) {
  *size = 0;

  printf("$ ");

  getline(input, size, stdin);

  // remove trailing newline
  size_t len = strlen(*input);
  if (len > 0 && (*input)[len - 1] == '\n') {
    (*input)[len - 1] = '\0';
  }
}

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  char *input = NULL;
  size_t size = 0;
  while (true) {
    collect_input(&input, &size);
    if (strcmp(input, "exit") == 0) {
      break;
    }
    printf("%s: command not found\n", input);
  }

  free(input);
  return 1;
}
