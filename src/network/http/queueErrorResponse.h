#ifndef HTTP_QUEUE_ERROR_RESPONSE_H
#define HTTP_QUEUE_ERROR_RESPONSE_H

#include <microhttpd.h>

int queue_error_response(struct MHD_Connection *connection, char *error_msg);

#endif