#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <avahi-client/lookup.h>
#include <avahi-client/client.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/error.h>
#include <avahi-common/defs.h>
#include "../../structs/rpiNode.h"
#include "../../utils/signalHandler.h"
#include "mDnsScanCallbacks.h"

#define SERVICE_TYPE "_rpinode._tcp"

void* scan_mdns_service(void *arg) {
    //--------------------------------------------------------------------------
    // Run Check
    //--------------------------------------------------------------------------
    pthread_mutex_lock(&rpiNode.lock);
    if (rpiNode.config.mDNS < 1)
        return NULL;
    pthread_mutex_unlock(&rpiNode.lock);
    //--------------------------------------------------------------------------
    // Variables
    //--------------------------------------------------------------------------
    int error;
    pid_t t_pid = syscall(SYS_gettid);
    AvahiClient *client;
    AvahiServiceBrowser *sb;

    // stdout Notification
    printf("[mDNS][%d]: Started\n", t_pid);

    pthread_mutex_lock(&lock);
    //--------------------------------------------------------------------------
    // Create Poll
    //--------------------------------------------------------------------------
    simple_poll = avahi_simple_poll_new();
    if (!simple_poll) {
        fprintf(stderr, "[mDNS][%d]: Failed to create poll loop\n", t_pid);
        return NULL;
    }
    //--------------------------------------------------------------------------
    //  Set Client
    //--------------------------------------------------------------------------
    client = avahi_client_new(
        avahi_simple_poll_get(simple_poll), 0,
        client_callback, NULL, &error
    );
    if (!client) {
        fprintf(stderr, "[mDNS][%d]: Failed to create client: %s\n", t_pid, avahi_strerror(error));
        return NULL;
    }
    //--------------------------------------------------------------------------
    //  Create Browser
    //--------------------------------------------------------------------------
    sb = avahi_service_browser_new(
        client, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,
        SERVICE_TYPE, NULL, 0, browse_callback, client
    );
    if (!sb) {
        fprintf(stderr, "[mDNS][%d]: Failed to create browser: %s\n", t_pid, avahi_strerror(avahi_client_errno(client)));
        return NULL;
    }
    pthread_mutex_unlock(&lock);

    avahi_simple_poll_loop(simple_poll);

    //--------------------------------------------------------------------------
    //  Clean Up
    //--------------------------------------------------------------------------
    avahi_service_browser_free(sb);
    avahi_client_free(client);
    avahi_simple_poll_free(simple_poll);
    // Cleanup list
    pthread_mutex_lock(&rpiNode.lock);
    /*
    printf("\n=== 🔍 Current Services ===\n");
    ServiceInfo *s = rpiNode.service_list;
    while (s) {
        printf(" - %s @ %s:%d (%s)\n", s->name, s->address, s->port, s->host);
        s = s->next;
    }
    printf("===========================\n\n");
    */
    while (rpiNode.service_list) {
        ServiceInfo *tmp = rpiNode.service_list;
        rpiNode.service_list = rpiNode.service_list->next;
        free(tmp);
    }
    pthread_mutex_unlock(&rpiNode.lock);
    //--------------------------------------------------------------------------
    //  Complete
    //--------------------------------------------------------------------------
    printf("[mDNS][%d]: Stopped\n", t_pid);

    return NULL;

}


