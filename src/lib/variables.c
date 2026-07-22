// Copyright (c) 2026 Konrad Guzek

#include "src/lib/variables.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/lib/config.h"
#include "src/lib/vector.h"

static struct variable_vec *variables = NULL;

int declare_variable(const char *command_name, const char *type_option,
                     const char *declaration) {
  // fprintf(stderr, "%s: %s: invalid option '%s'\n", PROGRAM_NAME,
  // command_name,
  //         type_option);
  enum VARIABLE_TYPE var_type = VAR_TYPE_STRING;
  struct variable_definition *variable =
      malloc(sizeof(struct variable_definition));
  variable->type = var_type;
  char *name = strdup(declaration);
  char *value = strchr(name, '=');
  if (value == NULL) {
    fprintf(stderr, "%s: %s: invalid variable declaration '%s'\n", PROGRAM_NAME,
            command_name, declaration);
    return EXIT_FAILURE;
  }
  *(value++) = '\0';
  variable->name = name;
  variable->value = value;
  return upsert_variable(variable);
}

static int upsert_variable(struct variable_definition *variable) {
  size_t variables_size = variable_vec_size(variables);
  struct variable_definition *current;
  for (size_t i = 0; i < variables_size; i++) {
    current = variables->value[i];
    if (current == NULL || !variable_equal(variable, current)) {
      continue;
    }
    free(current);
    variables->value[i] = variable;
    return EXIT_SUCCESS;
  }
  return push_back_variable(&variables, variable);
}

struct variable_vec *get_declared_variables() {
  return variables;
}
