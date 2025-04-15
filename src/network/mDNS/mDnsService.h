#ifndef MDNS_SERVICE_H
#define MDSN_SERVICE_H

#include <stdint.h>
#include <sys/types.h>

int publish_mdns_service(uint16_t port, pid_t t_pid, char *cluster_id);
void stop_mdns_service();

#endif 