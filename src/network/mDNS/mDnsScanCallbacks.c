#include <string.h>
#include <avahi-client/lookup.h>
#include <avahi-common/error.h>
#include "../../utils/signalHandler.h"
#include "mDnsScanUpdateList.h"
//---------------------------------------------------------------
// Resolve Callback
//---------------------------------------------------------------
void resolve_callback(AvahiServiceResolver *r,
                      AvahiIfIndex interface,
                      AvahiProtocol protocol,
                      AvahiResolverEvent event,
                      const char *name,
                      const char *type,
                      const char *domain,
                      const char *host_name,
                      const AvahiAddress *address,
                      uint16_t port,
                      AvahiStringList *txt,
                      AvahiLookupResultFlags flags,
                      void *userdata) {

    if (event == AVAHI_RESOLVER_FOUND) {
        char addr[AVAHI_ADDRESS_STR_MAX];
        avahi_address_snprint(addr, sizeof(addr), address);
        printf("%s - %s - %s - %i\n", type, name, addr, port);
        if (strcmp(addr, "127.0.0.1") != 0 && strcmp(addr, "::1") != 0)
            add_service(name, host_name, addr, port);
    }

    avahi_service_resolver_free(r);

}
//---------------------------------------------------------------
// Browse Callback
//---------------------------------------------------------------
void browse_callback(AvahiServiceBrowser *b,
                     AvahiIfIndex interface,
                     AvahiProtocol protocol,
                     AvahiBrowserEvent event,
                     const char *name,
                     const char *type,
                     const char *domain,
                     AvahiLookupResultFlags flags,
                     void *userdata) {

    AvahiClient *client = userdata;

    if (event == AVAHI_BROWSER_NEW) {
        avahi_service_resolver_new(client, interface, protocol,
                                   name, type, domain,
                                   AVAHI_PROTO_UNSPEC, 0,
                                   resolve_callback, NULL);
    } else if (event == AVAHI_BROWSER_REMOVE) {
        // Cannot get address here, so we remove by name+host later
        // We’ll try just removing by name only
        remove_service(name, "unknown");
        //printf("removed - %s\n", name);
    } else if (event == AVAHI_BROWSER_FAILURE) {
        fprintf(stderr, "Browser failure: %s\n", avahi_strerror(avahi_client_errno(client)));
        avahi_simple_poll_quit(simple_poll);
    }    

}
//---------------------------------------------------------------
// Client Callback
//---------------------------------------------------------------
void client_callback(AvahiClient *c, AvahiClientState state, void *userdata) {
    if (state == AVAHI_CLIENT_FAILURE) {
        fprintf(stderr, "Client failure: %s\n", avahi_strerror(avahi_client_errno(c)));
        avahi_simple_poll_quit(simple_poll);
    }
}
