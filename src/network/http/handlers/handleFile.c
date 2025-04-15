#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "handleFile.h"
#include "../queueErrorResponse.h"
#include "../../../utils/readFile.h"
#include "../../../utils/strReplace.h"

int handle_File(struct MHD_Connection *connection, char *filename, char *contentType) {

    // Read the file
    char *file_contents = read_file(filename);
    if (!file_contents) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to load file: %s", filename);
        return queue_error_response(connection, error_msg);
    }

    char *final_output = str_replace(file_contents, "{{TEST_VAR1}}", "Yay... it worked");
    free(file_contents);  // original content no longer needed

    if (!final_output) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg), "Failed to process file: %s", filename);
        return queue_error_response(connection, error_msg);
    }

    // Create and send response
    struct MHD_Response *response = MHD_create_response_from_buffer(
        strlen(final_output), (void *)final_output, MHD_RESPMEM_MUST_FREE
    );
    MHD_add_response_header(response, "Content-Type", contentType);
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    MHD_add_response_header(response, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    MHD_add_response_header(response, "Access-Control-Allow-Headers", "Content-Type");

    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;

}