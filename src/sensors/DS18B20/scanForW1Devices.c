#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>  // For PATH_MAX
#include "../../structs/rpiNode.h"
#include "readSlave.h"

void scanFor_w1_devices(pid_t t_pid) {

    const char *directory_path = "/sys/bus/w1/devices/";
    const char *prefix = "28-"; //DS18B20 family group prefix

    DIR *dir = opendir(directory_path);
    if (dir) {
        
        int index = 0;
        struct dirent *entry;
        char full_path[PATH_MAX];
        
        while ((entry = readdir(dir)) != NULL) {
            // Skip "." and ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            // Found device entries
            if (strncmp(entry->d_name, prefix, strlen(prefix)) == 0) {
                snprintf(full_path, sizeof(full_path), "%s/%s/w1_slave", directory_path, entry->d_name);
                read_slave(index, entry->d_name, full_path, t_pid);
                index += 1;
            }
        }

        pthread_mutex_lock(&rpiNode.lock);
        rpiNode.internal.DS18B20_count = index;
        rpiNode.internal.DS18B20[index].values_count = 2;
        pthread_mutex_unlock(&rpiNode.lock);
        
        closedir(dir);

    } else {
        fprintf(stderr, "[DS18B20][%d]: opendir: %s\n", t_pid, strerror(errno));
    }

}
