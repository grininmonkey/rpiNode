#include <string.h>
#include <stdlib.h>
#include "handleFile.h"
#include "../../../utils/readFile.h"
#include "../../../utils/strReplace.h"

int handle_File(struct MHD_Connection *connection, char *filename, char *contentType) {

    // Read the HTML file
    char *file_contents = read_file(filename);
    if (!file_contents) {
        const char *error = "Failed to load file.";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error),
                                            (void *)error, MHD_RESPMEM_PERSISTENT);
        int ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
        MHD_destroy_response(response);
        return ret;
    }

    char *final_output = str_replace(file_contents, "{{TEST_VAR1}}", "Yay... it worked");
    free(file_contents);  // original content no longer needed

    if (!final_output) {
        const char *error = "Failed to process file.";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error),
                                            (void *)error, MHD_RESPMEM_PERSISTENT);
        int ret = MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response);
        MHD_destroy_response(response);
        return ret;
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