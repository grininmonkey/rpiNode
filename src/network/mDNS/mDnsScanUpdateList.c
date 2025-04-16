#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "../../structs/rpiNode.h"
//---------------------------------------------------------------
// Add to service list
//---------------------------------------------------------------
void add_service(const char *name, const char *host, const char *addr, uint16_t port) {
    pthread_mutex_lock(&rpiNode.lock);

    // Check if already exists
    ServiceInfo *s = rpiNode.service_list;
    while (s) {
        if (strcmp(s->name, name) == 0 && strcmp(s->address, addr) == 0) {
            pthread_mutex_unlock(&rpiNode.lock);
            return; // already exists
        }
        s = s->next;
    }

    // Add new
    ServiceInfo *new_service = malloc(sizeof(ServiceInfo));
    strncpy(new_service->name, name, sizeof(new_service->name));
    strncpy(new_service->host, host, sizeof(new_service->host));
    strncpy(new_service->address, addr, sizeof(new_service->address));
    new_service->port = port;
    new_service->next = rpiNode.service_list;
    rpiNode.service_list = new_service;

    pthread_mutex_unlock(&rpiNode.lock);
}
//---------------------------------------------------------------
// Remove from service list
//---------------------------------------------------------------
void remove_service(const char *name, const char *host) {
    pthread_mutex_lock(&rpiNode.lock);

    ServiceInfo **curr = &rpiNode.service_list;
    while (*curr) {
        if (strcmp((*curr)->name, name) == 0) {
            ServiceInfo *to_delete = *curr;
            *curr = (*curr)->next;
            free(to_delete);
        } else {
            curr = &((*curr)->next);
        }
    }

    pthread_mutex_unlock(&rpiNode.lock);
}
