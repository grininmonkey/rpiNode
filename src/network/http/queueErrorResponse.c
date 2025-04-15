#include <string.h>
#include "queueErrorResponse.h"

int queue_error_response(struct MHD_Connection *connection, char *error_msg) {

    struct MHD_Response *response = MHD_create_response_from_buffer(
        strlen(error_msg), (void *)error_msg, MHD_RESPMEM_PERSISTENT
    );
    int ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
    MHD_destroy_response(response);
    return ret;

}