#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/syscall.h>
#include "../../structs/rpiNode.h"
#include "../../utils/signalHandler.h"
#include "../../utils/sleepMs.h"
#include "answerConnection.h"
#include "../mDNS/mDnsService.h"

#define BROADCAST_SEND_COUNTER 4

void* serve_http(void* arg) {
    //--------------------------------------------------------------------------
    // Vars
    //--------------------------------------------------------------------------
    int sockfd, publish_mDNS, broadcast_enable, counter = 0;
    int broadcast_interval = 2 * 10; // 10 loops/sec default 2s  TODO: Add to Config                 
    char MESSAGE[sizeof(((Config *)0)->id) + 15];
    char service_name[sizeof(((Config *)0)->id) + 1];
    pid_t p_tid = syscall(SYS_gettid);
    struct sockaddr_in broadcast_addr;
    unsigned int port;                  
    //--------------------------------------------------------------------------
    // Run Check & initial assignments
    //--------------------------------------------------------------------------
    pthread_mutex_lock(&rpiNode.lock);
        if (rpiNode.config.http < 1 || rpiNode.config.httpPort < 80) {
            pthread_mutex_unlock(&rpiNode.lock);
            return NULL;
        }
        port = (unsigned int)rpiNode.config.httpPort;   // Configure http port
        publish_mDNS = rpiNode.config.mDNS;             // mDNS flag
        broadcast_enable = rpiNode.config.broadcast;    // UDP flag
        snprintf(service_name, sizeof(service_name), "%s", rpiNode.config.id);
        // Configure the UDP broadcast address and message
        if (broadcast_enable) {
            memset(&broadcast_addr, 0, sizeof(broadcast_addr));
            broadcast_addr.sin_family = AF_INET;
            broadcast_addr.sin_port = htons(rpiNode.config.broadcastPort);
            broadcast_addr.sin_addr.s_addr = inet_addr(rpiNode.config.broadcastIP);
            snprintf(MESSAGE, sizeof(MESSAGE), "rpiNodeID:%s", rpiNode.config.id);        
        }
    pthread_mutex_unlock(&rpiNode.lock);
    //--------------------------------------------------------------------------
    // Create UDP socket and Enable broadcast
    //--------------------------------------------------------------------------
    if (broadcast_enable) {
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
            perror("setsockopt failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }
    // stdout Notification
    printf("[NETWORK][%d]: http started on port(%i)\n", p_tid, port);
    //--------------------------------------------------------------------------
    // Start Micro HTTP daemon-server & mDNS
    //--------------------------------------------------------------------------
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, port, NULL, NULL,
        (MHD_AccessHandlerCallback)&answer_connection, NULL, MHD_OPTION_END);

    if (NULL == daemon) {
        printf("[NETWORK][%d]: http stopped, unable to start daemon\n", p_tid);
        return NULL;
    } 
    
    if (publish_mDNS > 0 && publish_mdns_service(port, p_tid, service_name) != 0)
        fprintf(stderr, "[NETWORK][%d]: Failed to publish mDNS service.\n", p_tid);
    //--------------------------------------------------------------------------
    // thread loop
    //--------------------------------------------------------------------------
    while (1) {
        pthread_mutex_lock(&lock);
        if (!running) {
            pthread_mutex_unlock(&lock);
            break;
        }
        pthread_mutex_unlock(&lock);

        if (broadcast_enable)
            if (counter % broadcast_interval == 0)
                sendto(
                    sockfd, MESSAGE, strlen(MESSAGE), 0,
                    (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)
                );

        counter = (counter + 1) % 1000000;
        sleep_ms(100);

    }
    //--------------------------------------------------------------------------
    // Clean up
    //--------------------------------------------------------------------------
    close(sockfd);
    stop_mdns_service(p_tid);
    MHD_stop_daemon(daemon);
    printf("[NETWORK][%d]: Stopped\n", p_tid);

    return NULL;
}