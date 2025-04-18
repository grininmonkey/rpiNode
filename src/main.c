#include <string.h>
#include <unistd.h>
#include "config/configFileRead.h"
#include "structs/rpiNode.h"
#include "utils/setTmpfs.h"
#include "utils/nameValue.h"
#include "utils/signalHandler.h"
#include "local/db/initDB.h"
#include "local/db/writeToDB.h"
#include "sensors/DS18B20/readDS18B20.h"
#include "sensors/MPU6050/readMPU6050.h"
#include "network/http/serveHttp.h"
#include "network/mDNS/mDnsScan.h"
#include "data/freeData.h"

#define NUM_SERVICE_THREADS 5
//--------------------------------------------------------------------------------
// Set some config defaults in case config.json is missing/empty
//--------------------------------------------------------------------------------
SharedData rpiNode = {
    .config = {
        .id = "rpiNode-id-notset",
        .clusterID = "7B28F91B307D4AD5BF586057CEAF010A",
        .http = 1,
        .mDNS = 1,
        .httpPort = 80,
        .useTmpfs = 0,
        .tmpfsSize = 1,
        .broadcast = 0,
        .updateDBSeconds = 2,
        .tmpfsFolderName = "tmpfs",
        .currentValuesDBName = "current",
        .currentValuesViewName = "vValues"
    },
    .data = {{0}},
    .internal.DS18B20 = {{0}},
    .internal_config = NULL,
    .service_list = NULL,
    .lock = PTHREAD_MUTEX_INITIALIZER
};
//--------------------------------------------------------------------------------
// Function pointer array
//--------------------------------------------------------------------------------
void* (*service_thread_functions[NUM_SERVICE_THREADS])(void*) = {
    // Core threads
    serve_http,
    write_to_db,
    scan_mdns_service,
    // Internal code/threads for sensors etc
    read_ds18b20,
    read_mpu6050
};
//--------------------------------------------------------------------------------
// Launch threads based on flags 
//--------------------------------------------------------------------------------
void launch_service_threads() {
    pthread_t threads[NUM_SERVICE_THREADS];
    for (int i = 0; i < NUM_SERVICE_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, service_thread_functions[i], NULL) != 0) {
            perror("pthread_create");
        }
    }
    // Join active threads
    for (int i = 0; i < NUM_SERVICE_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}
//--------------------------------------------------------------------------------
// Main
//--------------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s --service | --get-tag node-id=<nodeID> tag-id=<tagID> ....\n", argv[0]);
        return 1;
    }

    pid_t m_pid = getpid();
    
    // Register signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    if (strcmp(argv[1], "--service") == 0) {
        //--------------------------------------------
        // service/server mode
        //--------------------------------------------
        printf("[main][%d]: Started\n", m_pid);
        if (
            config_file_read(m_pid) 
            && setTmpfs(m_pid)
            && initializeDB(m_pid)
        ) 
            launch_service_threads();
        
        // Clean up
        pthread_mutex_destroy(&lock);
        pthread_mutex_destroy(&rpiNode.lock);
        free_data();
        namevalue_free_all(rpiNode.internal_config);
        printf("[main][%d]: Stopped\n", m_pid);
    } else {
        // run future client function
    }

    return 0;

}

