#include <stdio.h>       // for printf()
#include <unistd.h>      // for syscall()
#include <sys/syscall.h> // for SYS_gettid
#include "dbWriteTags.h"
#include "../utils/sleepMs.h"
#include "../structs/rpiNode.h"
#include "../utils/signalHandler.h"

void *db_save_values_thread(void *args) {
    int counter = 0;
    int trigger_interval = 2 * 10; // 10 loops/sec default 2s
    pid_t t_pid = syscall(SYS_gettid);

    pthread_mutex_lock(&rpiNode.lock); 
    if (rpiNode.config.saveToDB != 1) {
        pthread_mutex_unlock(&rpiNode.lock); 
        return NULL;
    }
    trigger_interval = rpiNode.config.updateDBSeconds * 10; // 10 loops/sec
    printf("[DB][%d]: Started -> write to DB every (%i)s\n", t_pid, rpiNode.config.updateDBSeconds);
    pthread_mutex_unlock(&rpiNode.lock); 

    while (1) {
        pthread_mutex_lock(&lock);
        if (!running) {
            pthread_mutex_unlock(&lock);
            break;
        }
        pthread_mutex_unlock(&lock);

        if (counter % trigger_interval == 0)
            db_write_tags(t_pid);

        counter = (counter + 1) % 1000000;
        sleep_ms(100);
    }

    printf("[DB][%d]: Write to DB Stopped\n", t_pid);

    return NULL;
}