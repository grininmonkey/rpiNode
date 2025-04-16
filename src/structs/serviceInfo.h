#include <stdint.h>
#include <avahi-client/client.h>

typedef struct ServiceInfo {
    char name[256];
    char host[256];
    char address[AVAHI_ADDRESS_STR_MAX];
    uint16_t port;
    struct ServiceInfo *next;
} ServiceInfo;