#include <string.h>
#include <stdlib.h>

char *str_replace(const char *orig, const char *rep, const char *with) {
    char *result, *ins, *tmp;
    size_t len_rep, len_with, len_front, count;

    if (!orig || !rep || !with) return NULL;

    len_rep = strlen(rep);
    len_with = strlen(with);
    ins = (char *)orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);
    if (!result) return NULL;

    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = memcpy(tmp, orig, len_front) + len_front;
        tmp = memcpy(tmp, with, len_with) + len_with;
        orig += len_front + len_rep;
    }
    strcpy(tmp, orig);
    return result;
}
