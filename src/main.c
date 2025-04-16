#include <string.h>
#include <unistd.h>
#include "config/readConfigFile.h"
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

#define NUM_DAEMON_THREADS 5
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
        .DS18B20scanSeconds = 5,
        .currentValuesDBName = "current",
        .currentValuesViewName = "vValues",
        .MPU6050scanMilliseconds = 500
    },
    .internal_config = NULL,
    .service_list = NULL,
    .lock = PTHREAD_MUTEX_INITIALIZER
};
//--------------------------------------------------------------------------------
// Function pointer array
//--------------------------------------------------------------------------------
void* (*daemon_thread_functions[NUM_DAEMON_THREADS])(void*) = {
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
void launch_daemon_threads() {
    pthread_t threads[NUM_DAEMON_THREADS];
    for (int i = 0; i < NUM_DAEMON_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, daemon_thread_functions[i], NULL) != 0) {
            perror("pthread_create");
        }
    }
    // Join active threads
    for (int i = 0; i < NUM_DAEMON_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}
//--------------------------------------------------------------------------------
// Main
//--------------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s --daemon | --get-tag node-id=<nodeID> tag-id=<tagID> ....\n", argv[0]);
        return 1;
    }

    pid_t m_pid = getpid();
    
    // Register signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    if (strcmp(argv[1], "--daemon") == 0) {
        //--------------------------------------------
        // daemon mode
        //--------------------------------------------
        printf("[main][%d]: Started\n", m_pid);
        if (
            read_config_file(m_pid) 
            && setTmpfs(m_pid)
            && initializeDB(m_pid)
        ) 
            launch_daemon_threads();
        
        // Clean up
        pthread_mutex_destroy(&lock);
        pthread_mutex_destroy(&rpiNode.lock);
        namevalue_free_all(rpiNode.internal_config);
        printf("[main][%d]: Stopped\n", m_pid);
    } else {
        // run future client function
    }

    return 0;

}

