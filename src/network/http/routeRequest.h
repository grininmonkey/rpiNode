#ifndef ROUTE_REQUEST_H
#define ROUTE_REQUEST_H

#include <microhttpd.h>

int route_request(const char *url, struct MHD_Connection *connection);

#endif