#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../structs/rpiNode.h"

int setTmpfs(pid_t pid) {

    char mount_point[500];  // Path where tmpfs will be mounted
    char size_m[15];        // Size of tmpfs in M

    pthread_mutex_lock(&rpiNode.lock);
    sprintf(mount_point, "%s/%s", RPI_CONFIG_SERVE_PATH, rpiNode.config.tmpfsFolderName);
    umount(mount_point);
    if (rpiNode.config.useTmpfs != 1) {
        pthread_mutex_unlock(&rpiNode.lock);
        printf("[main][%d]: useTmpfs=false, not creating tmpfs...\n", pid);
        return 1; 
    }
    pthread_mutex_unlock(&rpiNode.lock);

    // Creating the mount point directory if it does not exist
    if (access(mount_point, F_OK) == -1) {
        if(mkdir(mount_point, 0755) == -1) {
            perror("mkdir failure");
            return 0;
        }
    }  

    snprintf(size_m, sizeof(size_m), "size=%iM", rpiNode.config.tmpfsSize);

    // Mount tmpfs with a size limit
    if (mount("tmpfs", mount_point, "tmpfs", MS_NOATIME, size_m) == -1) {
        perror("mount");
        return 0;
    }

    printf("[main][%d]: tmpfs mounted successfully at %s\n", pid, mount_point);
    
    return 1;
}

