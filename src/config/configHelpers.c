#include <string.h>
#include <unistd.h>
#include <jansson.h>
#include "../structs/rpiNode.h"

int test_string(const char *name, json_t *obj, int MAX) {
    if (json_is_string(obj)) {
        if (INT_GT_ZERO_AND_LE(strlen(json_string_value(obj)), MAX)) {
            return 1;
        } else {
            fprintf(stderr, 
                "[main][%d]: config <%s> length (%i) > max length (%i)\n",
                getpid(), name, strlen(json_string_value(obj)), MAX
            );
        }
    }
    return 0;
}

const char* get_string(const char *name, const char *current, json_t *new, int MAX) {
    if (test_string(name, new, MAX))
        return json_string_value(new);
    return current;
}

int get_true_false(int current, json_t *obj) {
    if (json_is_integer(obj))
        return INT_TRUE_FALSE(json_integer_value(obj));
    return current;
}

int get_integer(int current, json_t *obj) {
    if (json_is_integer(obj))
        return json_integer_value(obj);
    return current;
}
