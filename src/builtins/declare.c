// Copyright (c) 2026 Konrad Guzek

#include "src/builtins/declare.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/config.h"
#include "src/lib/variables.h"
#include "src/lib/vector.h"

int builtin_declare(size_t argc, const char *argv[argc]) {
  const char *command_name = argv[0];
  if (argc == 1) {
    fprintf(stderr, "%s: %s: missing option\n", PROGRAM_NAME, command_name);
    return EXIT_FAILURE;
  }
  const char *option = argv[1];
  if (strcmp(option, "-p") == 0) {
    int result = EXIT_SUCCESS;
    for (size_t i = 2; i < argc; i++) {
      result = print_declared_variable(command_name, argv[i]);
    }
    return result;
  }
  return declare_variable(command_name, NULL, option);
  return EXIT_FAILURE;
}

static int print_declared_variable(const char *command_name,
                                   const char *variable_name) {
  struct variable_vec *variables = get_declared_variables();
  size_t variables_size = variable_vec_size(variables);
  struct variable_definition *variable;
  for (size_t i = 0; i < variables_size; i++) {
    variable = variables->value[i];
    if (variable == NULL || strcmp(variable_name, variable->name) != 0) {
      continue;
    }
    switch (variable->type) {
    case VAR_TYPE_STRING:
      printf("%s -- %s=\"%s\"\n", command_name, variable->name,
             variable->value);
      break;
    default:
      fprintf(stderr, "%s: %s: unknown variable type '%d'", PROGRAM_NAME,
              command_name, variable->type);
      break;
    }
    return EXIT_SUCCESS;
  }
  fprintf(stderr, "%s: %s: not found\n", command_name, variable_name);
  return EXIT_FAILURE;
}
