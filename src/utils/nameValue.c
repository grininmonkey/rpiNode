#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "nameValue.h"

void namevalue_add(NameValue **head, const char *name, const char *value) {
    NameValue *current = *head;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            strncpy(current->value, value, RPI_MAX_NAMEVALUE_VALUE - 1);
            current->value[RPI_MAX_NAMEVALUE_VALUE - 1] = '\0';
            return;
        }
        current = current->next;
    }

    NameValue *new_node = (NameValue *)malloc(sizeof(NameValue));
    if (!new_node) return;

    strncpy(new_node->name, name, RPI_MAX_NAMEVALUE_NAME - 1);
    new_node->name[RPI_MAX_NAMEVALUE_NAME - 1] = '\0';

    strncpy(new_node->value, value, RPI_MAX_NAMEVALUE_VALUE - 1);
    new_node->value[RPI_MAX_NAMEVALUE_VALUE - 1] = '\0';

    new_node->next = *head;
    *head = new_node;
}

NameValue *namevalue_find(NameValue *head, const char *name) {
    while (head != NULL) {
        if (strcmp(head->name, name) == 0) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

int namevalue_find_as_integer(NameValue *head, const char *name, int *out_value) {
    NameValue *found = namevalue_find(head, name);
    if (!found || !found->value[0]) {
        return 0; // not found or empty string
    }

    // Try number first
    errno = 0;
    char *endptr;
    long val = strtol(found->value, &endptr, 10);
    if (errno == 0 && *endptr == '\0') {
        *out_value = (int)val;
        return 1;
    }

    // Case-insensitive check for "true" / "false"
    if (strcasecmp(found->value, "true") == 0) {
        *out_value = 1;
        return 1;
    }
    if (strcasecmp(found->value, "false") == 0) {
        *out_value = 0;
        return 1;
    }

    return 0; // failed to interpret
}

void namevalue_free_all(NameValue *head) {
    NameValue *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}