#include <string.h>
#include <jansson.h>
#include "handleStatus.h"
#include "../../../structs/rpiNode.h"
#include "../../../utils/getTimeStamp.h"

int handle_status(struct MHD_Connection *connection) {
    json_t *root = json_object();
    json_object_set_new(root, "status", json_string("OK"));
    json_object_set_new(root, "timestamp", json_string(get_timestamp()));
    //------------------------------------------------------------------------
    pthread_mutex_lock(&rpiNode.lock);
    
    json_object_set_new(root, "id", json_string(rpiNode.config.id));
    json_object_set_new(root, "http", json_boolean(rpiNode.config.http));
    json_object_set_new(root, "mDNS", json_boolean(rpiNode.config.mDNS));
    json_object_set_new(root, "master", json_boolean(rpiNode.config.master));
    json_object_set_new(root, "useTmpfs", json_boolean(rpiNode.config.useTmpfs));
    json_object_set_new(root, "saveToDB", json_boolean(rpiNode.config.saveToDB));
    json_object_set_new(root, "broadcast", json_boolean(rpiNode.config.broadcast));
    json_object_set_new(root, "DBcreated", json_boolean(rpiNode.config.DBcreated));
    json_object_set_new(root, "httpPort", json_integer(rpiNode.config.httpPort));
    json_object_set_new(root, "tmpfsSizeM", json_integer(rpiNode.config.tmpfsSize));
    json_object_set_new(root, "broadcastPort", json_integer(rpiNode.config.broadcastPort));
    json_object_set_new(root, "updateDBSeconds", json_integer(rpiNode.config.updateDBSeconds));
    json_object_set_new(root, "broadcastIP", json_string(rpiNode.config.broadcastIP));
    json_object_set_new(root, "tmpfsFolderName", json_string(rpiNode.config.tmpfsFolderName));
    json_object_set_new(root, "currentValuesDB", json_string(rpiNode.config.currentValuesDB));
    json_object_set_new(root, "currentValuesDBName", json_string(rpiNode.config.currentValuesDBName));
    json_object_set_new(root, "currentValuesViewName", json_string(rpiNode.config.currentValuesViewName));

    json_t *internal = json_object();
    json_object_set_new(internal, "MPU6050", json_boolean(rpiNode.config.MPU6050));
    json_object_set_new(internal, "MPU6050scanMilliseconds", json_integer(rpiNode.config.MPU6050scanMilliseconds));
    json_object_set_new(internal, "MPU6050valuesCount", json_integer(rpiNode.internal.MPU6050.values_count));
    json_object_set_new(internal, "DS18B20", json_boolean(rpiNode.config.DS18B20));
    json_object_set_new(internal, "DS18B20scanSeconds", json_integer(rpiNode.config.DS18B20scanSeconds));
    json_object_set_new(internal, "DS18B20count", json_integer(rpiNode.internal.DS18B20_count));
    
    pthread_mutex_unlock(&rpiNode.lock);
    //------------------------------------------------------------------------

    json_object_set_new(root, "internal", internal);

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