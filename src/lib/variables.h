// Copyright (c) 2026 Konrad Guzek

#ifndef SRC_LIB_VARIABLES_H_
#define SRC_LIB_VARIABLES_H_

enum VARIABLE_TYPE {
  VAR_TYPE_STRING,
};

struct variable_definition {
  char *name;
  enum VARIABLE_TYPE type;
  char *value;
};

int declare_variable(const char *command_name, const char *type_option,
                     const char *declaration);
struct variable_vec *get_declared_variables();
char *get_variable_value(const char *variable_name);
bool is_valid_variable_char(const char c);
static int upsert_variable(struct variable_definition *variable);
static bool is_valid_variable_name(const char *name);
static bool is_letter_or_underscore(const char c);
static bool is_digit(const char c);

#endif  // SRC_LIB_VARIABLES_H_
