#include <string.h>
#include "routeRequest.h"
//-----------------------------------------------------
//#include "handlers/values.h"
//#include "handlers/status.h"  // more handlers
//-----------------------------------------------------

int route_request(const char *url, struct MHD_Connection *connection) {

    /*
    if (strcmp(url, "/api/values") == 0) {
        return handle_values(connection);
    } else if (strcmp(url, "/api/status") == 0) {
        return handle_status(connection);
    }
    */

    // fallback
    const char *msg = "Not Found";
    struct MHD_Response *resp = MHD_create_response_from_buffer(strlen(msg), (void *)msg, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, resp);
    MHD_destroy_response(resp);
    return ret;

}