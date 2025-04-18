#include <stdio.h>
#include <string.h>
#include "httpRouteRequest.h"
#include "httpQueueErrorResponse.h"
//-----------------------------------------------------
//#include "handlers/<name>.h"
#include "handlers/httpHandleStatus.h"  
#include "handlers/httpHandleFile.h"
//-----------------------------------------------------

//
// TODO: 
//      base filename path as a define somewhere, or make
//      it a config variable?
//

int http_route_request(const char *url, struct MHD_Connection *connection) {

    // TODO: Add /api/<json> handlers
    if (strcmp(url, "/api/status") == 0)
        return http_handle_status(connection);

    //TODO: determine content types, add generic handle_file call based on url var etc...
    if (strcmp(url, "/") == 0 || strcmp(url, "/index.html") == 0)
        return http_handle_File(connection,"/srv/rpiNode/html/index.html","text/html");



    // fallback for now -- unreached once generic file handler is implemented
    char error_msg[256];
    snprintf(error_msg, sizeof(error_msg), "%s&nbsp;:&nbsp;Not Found", url);
    return http_queue_error_response(connection, error_msg, MHD_HTTP_NOT_FOUND);

}