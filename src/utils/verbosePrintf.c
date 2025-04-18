#include <stdio.h>
#include <string.h>
#include "../structs/rpiNode.h"


void verbose_printf(const char *format, ...) {
    if (!rpiNode.verbose) {
        return;
    }

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void verbose_mutex_printf(const char *format, ...) {
    pthread_mutex_lock(&rpiNode.lock);
        if (!rpiNode.verbose) {
            pthread_mutex_unlock(&rpiNode.lock);    
            return;
        }
    pthread_mutex_unlock(&rpiNode.lock);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}