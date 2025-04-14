#include <string.h>
#include <jansson.h>
#include "handleStatus.h"

int handle_status(struct MHD_Connection *connection) {
    json_t *root = json_object();
    json_object_set_new(root, "status", json_string("OK"));

    char *json = json_dumps(root, 0);
    json_decref(root);    

    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(json), json, MHD_RESPMEM_MUST_FREE);
    MHD_add_response_header(response, "Content-Type", "application/json");
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    MHD_add_response_header(response, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    MHD_add_response_header(response, "Access-Control-Allow-Headers", "Content-Type");

    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;    

}