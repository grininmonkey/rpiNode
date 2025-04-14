#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getLineFromCmd.h"

int get_line_from_cmd(const char *cmd, char *output, size_t output_size) {
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
        perror("popen failed");
        return -1;
    }

    // Read only the first line
    if (fgets(output, output_size, fp) == NULL) {
        pclose(fp);
        return -1;
    }

    // Strip trailing newline, if any
    output[strcspn(output, "\n")] = '\0';

    return pclose(fp);
}

