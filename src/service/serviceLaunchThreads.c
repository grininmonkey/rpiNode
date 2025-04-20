#include <stdio.h>
#include <unistd.h>
#include "../config/configFileRead.h"
#include "../data/dataFree.h"
#include "../db/dbInitalize.h"
#include "../db/dbSaveValuesThread.h"
#include "../network/http/httpServeThread.h"
#include "../network/mDNS/mDnsScanServiceThread.h"
// soon to be obsolete ....................
#include "../sensors/DS18B20/readDS18B20.h"
#include "../sensors/MPU6050/readMPU6050.h"
// ........................................
#include "../structs/rpiNode.h"
#include "../utils/setTmpfs.h"
#include "../utils/nameValue.h"
#include "../utils/signalHandler.h"
#include "../utils/verbosePrintf.h"
#include "../utils/checkServiceRunning.h"
//---------------------------------------------------------------
// Define the number of threads to launch
//---------------------------------------------------------------

#define NUM_SERVICE_THREADS 5

//---------------------------------------------------------------
// Thread Function(s) Array
//---------------------------------------------------------------
void* (*service_thread_functions[NUM_SERVICE_THREADS])(void*) = {
    //........................
    // Core threads
    //........................
    http_serve_thread,
    db_save_values_thread,
    mdns_scan_service_thread,
    //........................
    // Module Threads
    //........................
    read_ds18b20,
    read_mpu6050
};
//---------------------------------------------------------------
// Function pointer array
//---------------------------------------------------------------
void service_launch_threads(int argc, char *argv[], pid_t m_pid) {
    //--------------------------------------------
    // Check if already running service instance
    //--------------------------------------------
    if (check_service_running(m_pid)) {
        printf("Already running a service instance!\n");
        return;
    }
    //--------------------------------------------
    // Start msg
    //--------------------------------------------
    verbose_printf("[main][%d]: Started\n", m_pid);
    //--------------------------------------------
    // Initialize and Launch Service Threads
    //--------------------------------------------
    if (
        config_file_read(m_pid) 
        && setTmpfs(m_pid)
        && db_initalize(m_pid)
    ) {

        pthread_t threads[NUM_SERVICE_THREADS];
        // Loop thru Function(s) Array
        for (int i = 0; i < NUM_SERVICE_THREADS; i++) {
            if (pthread_create(&threads[i], NULL, service_thread_functions[i], NULL) != 0) {
                perror("service_launch_threads - pthread_create");
            }
        }
        // Join active threads
        for (int i = 0; i < NUM_SERVICE_THREADS; i++) {
            pthread_join(threads[i], NULL);
        }        

    }
    //--------------------------------------------
    // Clear Mutex(s)
    //--------------------------------------------
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&rpiNode.lock);
    //--------------------------------------------
    // Free memory allocations etc...
    //--------------------------------------------
    data_free();
    namevalue_free_all(rpiNode.internal_config);
    //--------------------------------------------
    // Exit msg
    //--------------------------------------------
    verbose_printf("[main][%d]: Stopped\n", m_pid);

}