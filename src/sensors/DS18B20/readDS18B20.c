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
#include "../../utils/nameValue.h"

void* read_ds18b20(void* arg) {
    //------------------------------------------------------------------------
    // Top Vars
    //------------------------------------------------------------------------
    int counter = 0;
    int trigger_interval = 5 * 10; // 10 loops/sec Default to 5
    int DS18B20, DS18B20scanSeconds;
    pid_t t_pid = syscall(SYS_gettid);
    //------------------------------------------------------------------------
    // Get/Set settings
    //------------------------------------------------------------------------
    pthread_mutex_lock(&rpiNode.lock);
        if (namevalue_find_as_integer(rpiNode.internal_config, "DS18B20", &DS18B20) == 0)
            DS18B20 = 0;
        if (namevalue_find_as_integer(rpiNode.internal_config, "DS18B20scanSeconds", &DS18B20scanSeconds) == 0)
            DS18B20scanSeconds = 5;
        if (DS18B20 != 1) {
            pthread_mutex_unlock(&rpiNode.lock);
            return NULL;
        }
        if (DS18B20scanSeconds > 0)
            trigger_interval = DS18B20scanSeconds * 10;
    pthread_mutex_unlock(&rpiNode.lock);
    //------------------------------------------------------------------------
    // Start
    //------------------------------------------------------------------------
    printf("[DS18B20][%d]: Started -> reading every (%i)s\n", t_pid, (trigger_interval/10));
    //------------------------------------------------------------------------
    // Loop forever
    //------------------------------------------------------------------------
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
        sleep_ms(trigger_interval);
    }
    //------------------------------------------------------------------------
    // Finish
    //------------------------------------------------------------------------
    printf("[DS18B20][%d]: Stopped\n", t_pid);

    return NULL;

}
