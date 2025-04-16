#include <stdint.h>
#include <avahi-client/client.h>
//-----------------------------------------------
// Pointer Struct for maintaining list of 
// discovered rpiNode services via avahi.
// Unknown count and no speed/cache concerns.
//-----------------------------------------------
typedef struct ServiceInfo {
    char name[256];
    char host[256];
    char address[AVAHI_ADDRESS_STR_MAX];
    uint16_t port;
    struct ServiceInfo *next;
} ServiceInfo;