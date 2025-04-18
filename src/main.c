#include <string.h>
#include <unistd.h>
#include "structs/rpiNode.h"
#include "utils/signalHandler.h"
#include "client/clientRouteInput.h"
#include "service/serviceLaunchThreads.h"
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
// Main
//--------------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    //-----------------------------------------------
    // Command line check
    //-----------------------------------------------
    if (argc < 2) {
        client_route_input_usage(argc, argv);
        return 1;
    }
    //-----------------------------------------------
    // Get main process ID
    //-----------------------------------------------
    pid_t m_pid = getpid();
    //-----------------------------------------------
    // Register signal handlers
    //-----------------------------------------------
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    //-----------------------------------------------
    // Route to Service or Client funcs
    //-----------------------------------------------
    if (strcmp(argv[1], "--service") == 0) {
        service_launch_threads(argc, argv, m_pid);
    } else {
        client_route_input(argc, argv);
    }
    //-----------------------------------------------
    // Completed
    //-----------------------------------------------
    return 0;

}

