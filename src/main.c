#include <unistd.h>
#include "config/readConfigFile.h"
#include "structs/rpiNode.h"
#include "utils/setTmpfs.h"
#include "utils/signalHandler.h"
#include "local/db/initDB.h"
#include "local/db/writeToDB.h"
#include "sensors/DS18B20/readDS18B20.h"
#include "sensors/MPU6050/readMPU6050.h"
#include "network/http/serveHttp.h"

#define NUM_THREADS 4
//--------------------------------------------------------------------------------
// Set Config Defaults
//--------------------------------------------------------------------------------
SharedData rpiNode = {
    .config = {
        .id = "rpiNode-id-notset",
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
    .lock = PTHREAD_MUTEX_INITIALIZER
};
//--------------------------------------------------------------------------------
// Function pointer array
//--------------------------------------------------------------------------------
void* (*thread_functions[NUM_THREADS])(void*) = {
    read_ds18b20,
    read_mpu6050,
    write_to_db,
    serve_http
};
//--------------------------------------------------------------------------------
// Launch threads based on flags 
//--------------------------------------------------------------------------------
void launch_threads() {
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, thread_functions[i], NULL) != 0) {
            perror("pthread_create");
        }
    }
    // Join active threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}
//--------------------------------------------------------------------------------
// Main
//--------------------------------------------------------------------------------
int main() {

    pid_t m_pid = getpid();
    
    // Register signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("[main][%d]: Started\n", m_pid);

    if (
           read_config_file(m_pid) 
        && setTmpfs(m_pid)
        && initializeDB(m_pid)
    ) 
        launch_threads();
    
    // Clean up
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&rpiNode.lock);

    printf("[main][%d]: Stopped\n", m_pid);

    return 0;

}

