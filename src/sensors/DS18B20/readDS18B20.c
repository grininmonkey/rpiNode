//#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include "scanForW1Devices.h"
#include "../../structs/rpiNode.h"
#include "../../utils/sleepMs.h"
#include "../../utils/signalHandler.h"

void* read_ds18b20(void* arg) {

    int counter = 0;
    int trigger_interval = 5 * 10; // 10 loops/sec Default to 5
    pid_t t_pid = syscall(SYS_gettid);

    pthread_mutex_lock(&rpiNode.lock);
    if (rpiNode.config.DS18B20 != 1) {
        pthread_mutex_unlock(&rpiNode.lock);
        return NULL;
    }
    if (rpiNode.config.DS18B20scanSeconds > 0)
        trigger_interval = rpiNode.config.DS18B20scanSeconds * 10;
    pthread_mutex_unlock(&rpiNode.lock);

    printf("[DS18B20][%d]: Started -> reading every (%i)s\n", t_pid, (trigger_interval/10));

    // Loop forever
    while (1) {
        pthread_mutex_lock(&lock);
        if (!running) {
            pthread_mutex_unlock(&lock);
            break;
        }
        pthread_mutex_unlock(&lock);

        if (counter % trigger_interval == 0)
            scanFor_w1_devices(t_pid);

        counter = (counter + 1) % 1000000;
        sleep_ms(100);
    }

    printf("[DS18B20][%d]: Stopped\n", t_pid);

    return NULL;

}
