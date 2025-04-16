#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../utils/setSharedValue.h"

void read_slave(int index, const char *device, const char *full_path, pid_t t_pid) {
    
    int values_count = 2;

    FILE *file = fopen(full_path, "r");
    if (!file) {
        fprintf(stderr, "[DS18B20][%d]: fopen: %s\n", t_pid, strerror(errno));
        return;
    }

    char line[256];
    double temperatureC = -1;
    double temperatureF = -1;

    while (fgets(line, sizeof(line), file)) {
        char *t_ptr = strstr(line, "t=");
        if (t_ptr) {
            temperatureC = atoi(t_ptr + 2)/1000.0;
            temperatureF = temperatureC* 9.0 / 5.0 + 32.0;
            pthread_mutex_lock(&rpiNode.lock);
                rpiNode.internal.DS18B20[index].values_count = 2;
            pthread_mutex_unlock(&rpiNode.lock);
            break;
        }
    }

    fclose(file);

    char id[RPI_MAX_META_ID];
    char value[RPI_MAX_VALUE_LENGTH];

    for (int i = 0; i < values_count; i++) {
        snprintf(id, RPI_MAX_META_ID, "%s-%s", device, (i == 0) ? "C" : "F");
        snprintf(value, RPI_MAX_VALUE_LENGTH, "%.2f", (i == 0) ? temperatureC : temperatureF);
        set_shared_multivalue(
            &rpiNode.internal.DS18B20[index], index, i, id, "1wire", "DS18B20 temperature sensor", 0.0, value
        );        
    }

    return;
}

