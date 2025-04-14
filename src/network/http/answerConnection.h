#ifndef ANSWER_CONNECTION_H
#define ANSWER_CONNECTION_H

#include <microhttpd.h>

int answer_connection(void *cls, struct MHD_Connection *connection,
    const char *url, const char *method,
    const char *version, const char *upload_data,
    size_t *upload_data_size, void **con_cls);

#endif