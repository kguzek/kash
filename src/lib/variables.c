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
    free(variable);
    return EXIT_FAILURE;
  }
  *(value++) = '\0';
  if (!is_valid_variable_name(name)) {
    // tests require this exact error format
    fprintf(stderr, "%s: `%s': not a valid identifier\n", command_name,
            declaration);
    free(variable);
    return EXIT_FAILURE;
  }
  variable->name = name;
  variable->value = value;
  return upsert_variable(variable);
}

struct variable_vec *get_declared_variables() {
  return variables;
}

char *get_variable_value(const char *variable_name) {
  if (strcmp(variable_name, "?") == 0) {
    char exit_code_str[8];
    snprintf(exit_code_str, sizeof(exit_code_str), "%d",
             get_previous_exit_code());
    return strdup(exit_code_str);
  }
  size_t variables_size = variable_vec_size(variables);
  struct variable_definition *variable;
  for (size_t i = 0; i < variables_size; i++) {
    variable = variables->value[i];
    if (variable == NULL || strcmp(variable_name, variable->name) != 0) {
      continue;
    }
    return variable->value;
  }
  return "";
}

bool is_valid_variable_char(const char c) {
  return is_letter_or_underscore(c) || is_digit(c);
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

static bool is_valid_variable_name(const char *name) {
  if (!is_letter_or_underscore(name[0])) {
    return false;
  }
  for (const char *c = name; *c != '\0'; c++) {
    if (is_valid_variable_char(*c)) {
      continue;
    }
    return false;
  }
  return true;
}

static bool is_letter_or_underscore(const char c) {
  if (c == '_') {
    return true;
  }
  if (c >= 'a' && c <= 'z') {
    return true;
  }
  if (c >= 'A' && c <= 'Z') {
    return true;
  }
  return false;
}

static bool is_digit(const char c) {
  return c >= '0' && c <= '9';
}
