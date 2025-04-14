#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../structs/rpiNode.h"
#include "../../utils/getTimeStamp.h"

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
            break;
        }
    }

    fclose(file);
    
    pthread_mutex_lock(&rpiNode.lock);
    for (int i = 0; i < values_count; i++) {
        snprintf(
            rpiNode.internal.DS18B20[index].values[i].info.id,
            sizeof(rpiNode.internal.DS18B20[0].values[i].info.id),
            "%s-%s", device, (i == 0) ? "C" : "F"
        );
        SAFE_STRCPY(rpiNode.internal.DS18B20[index].values[i].info.description,"DS18B20 temperature sensor");
        SAFE_STRCPY(rpiNode.internal.DS18B20[index].values[i].info.type,"1wire");
        SAFE_STRCPY(rpiNode.internal.DS18B20[index].values[i].info.timestamp, get_timestamp());
        snprintf(
            rpiNode.internal.DS18B20[index].values[i].value,
            sizeof(rpiNode.internal.DS18B20[0].values[i].value),
            "%.2f", (i == 0) ? temperatureC : temperatureF 
        );
    }
    rpiNode.internal.DS18B20[index].values_count = values_count;
    pthread_mutex_unlock(&rpiNode.lock);

    return;
}

