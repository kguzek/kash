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
static int upsert_variable(struct variable_definition *variable);
struct variable_vec *get_declared_variables();

#endif  // SRC_LIB_VARIABLES_H_
