#ifndef CONFIG_HELPERS_H
#define CONFIG_HELPERS_H

#include <jansson.h>

int get_integer(int current, json_t *obj);
int get_true_false(int current, json_t *obj);
int test_string(const char *name, json_t *obj, int MAX);
const char* get_string(const char *name, const char *current, json_t *new, int MAX);


#endif