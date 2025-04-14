#include <microhttpd.h>
#include "routeRequest.h"
#include "answerConnection.h"

int answer_connection(void *cls, struct MHD_Connection *connection,
    const char *url, const char *method,
    const char *version, const char *upload_data,
    size_t *upload_data_size, void **con_cls
) {
    return route_request(url, connection);
}