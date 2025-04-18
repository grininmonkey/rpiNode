#include <string.h>

int has_arg(int argc, char *argv[], const char *search_arg) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], search_arg) == 0) {
            return 1; // found
        }
    }
    return 0; // not found
}