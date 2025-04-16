#ifndef UTILS_NAMEVALUE_H
#define UTILS_NAMEVALUE_H

#include "../structs/rpiNode.h"

void namevalue_add(NameValue **head, const char *name, const char *value);
int namevalue_remove(NameValue **head, const char *name);
NameValue *namevalue_find(NameValue *head, const char *name);
int namevalue_find_as_integer(NameValue *head, const char *name, int *out_value);
void namevalue_free_all(NameValue *head);

#endif