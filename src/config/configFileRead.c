#include <stdio.h>
#include <string.h>
#include <jansson.h>
#include "configHelpers.h"
#include "configFileRead.h"
#include "configDataModules.h"
#include "../structs/rpiNode.h"
#include "../utils/nameValue.h"
#include "configFileCompleted.h"

int config_file_read(pid_t pid) {
    json_t *root;
    json_error_t error;
    //-----------------------------------------------------------
    // Load the JSON config file
    //-----------------------------------------------------------
    root = json_load_file(RPI_CONFIG_FILE, 0, &error);
    if (!root) {
        fprintf(stderr, "[main][%d]: Error loading config file: %s (line %d)\n",
            pid, error.text, error.line);
        return 0;
    }
    //-----------------------------------------------------------
    // Access data and set rpiNode.config and external commands
    //-----------------------------------------------------------
    json_t *id = json_object_get(root, "id");
    json_t *http = json_object_get(root, "http");
    json_t *mDNS = json_object_get(root, "mDNS");
    json_t *master = json_object_get(root, "master");
    json_t *serveMQ = json_object_get(root, "serveMQ");
    json_t *saveToDB = json_object_get(root, "saveToDB");
    json_t *httpPort = json_object_get(root, "httpPort");
    json_t *useTmpfs = json_object_get(root, "useTmpfs");
    json_t *tmpfsSize = json_object_get(root, "tmpfsSize");
    json_t *broadcast = json_object_get(root, "broadcast");
    json_t *clusterID = json_object_get(root, "clusterID");
    json_t *dataModules = json_object_get(root, "dataModules");
    json_t *broadcastIP = json_object_get(root, "broadcastIP");
    json_t *broadcastPort = json_object_get(root, "broadcastPort");
    json_t *tmpfsFolderName = json_object_get(root, "tmpfsFolderName");
    json_t *updateDBSeconds = json_object_get(root, "updateDBSeconds");
    json_t *internal_settings = json_object_get(root, "internal_settings");
    json_t *currentValuesDBName = json_object_get(root, "currentValuesDBName");

    pthread_mutex_lock(&rpiNode.lock);
    //-----------------------------------------------------------------
    // Base
    //-----------------------------------------------------------------
    SAFE_STRCPY(rpiNode.config.id, get_string("id", rpiNode.config.id, id, CONFIG_MAX_ID));
    SAFE_STRCPY(rpiNode.config.clusterID, get_string("clusterID", rpiNode.config.clusterID, clusterID, CONFIG_MAX_CLUSTER_ID));
    SAFE_STRCPY(rpiNode.config.broadcastIP, get_string("broadcastIP", rpiNode.config.broadcastIP, broadcastIP, CONFIG_MAX_BROADCASTIP));
    SAFE_STRCPY(rpiNode.config.tmpfsFolderName, get_string("tmpfsFolderName", rpiNode.config.tmpfsFolderName, tmpfsFolderName, CONFIG_MAX_TMPFSFOLDERNAME));
    SAFE_STRCPY(rpiNode.config.currentValuesDBName, get_string("currentValuesDBName", rpiNode.config.currentValuesDBName, currentValuesDBName, CONFIG_MAX_DB_NAME));
    
    rpiNode.config.http = get_true_false(rpiNode.config.http, http);
    rpiNode.config.mDNS = get_true_false(rpiNode.config.mDNS, mDNS); 
    rpiNode.config.master = get_true_false(rpiNode.config.master, master);
    rpiNode.config.serveMQ = get_true_false(rpiNode.config.serveMQ, serveMQ);
    rpiNode.config.useTmpfs = get_true_false(rpiNode.config.useTmpfs, useTmpfs);
    rpiNode.config.saveToDB = get_true_false(rpiNode.config.saveToDB, saveToDB);
    rpiNode.config.httpPort = get_integer(rpiNode.config.httpPort, httpPort);
    rpiNode.config.broadcast = get_true_false(rpiNode.config.broadcast, broadcast);
    rpiNode.config.tmpfsSize = get_integer(rpiNode.config.tmpfsSize, tmpfsSize);
    rpiNode.config.broadcastPort = get_integer(rpiNode.config.broadcastPort, broadcastPort);
    rpiNode.config.updateDBSeconds = get_integer(rpiNode.config.updateDBSeconds, updateDBSeconds);

    if (rpiNode.config.useTmpfs) {
        snprintf(rpiNode.config.currentValuesDB, CONFIG_MAX_DB, "%s/%s/%s.db", RPI_CONFIG_SERVE_PATH, rpiNode.config.tmpfsFolderName, rpiNode.config.currentValuesDBName);
    } else {
        snprintf(rpiNode.config.currentValuesDB, CONFIG_MAX_DB, "%s/%s.db", RPI_CONFIG_SERVE_PATH, rpiNode.config.currentValuesDBName);
    }
    //-----------------------------------------------------------------
    // Soon to be removed
    //-----------------------------------------------------------------
        size_t internal_index;
        json_t *element;
        if (json_is_array(internal_settings)) {
            json_array_foreach(internal_settings, internal_index, element) {
                json_t *jname = json_object_get(element, "name");
                json_t *jvalue = json_object_get(element, "value");
                if (json_is_string(jname) && json_is_string(jvalue)) {
                    namevalue_add(&rpiNode.internal_config, json_string_value(jname), json_string_value(jvalue));
                }
            }
        }
    //-----------------------------------------------------------------
    // DataModules pre populating rpiNode.data with 
    // DataModule Struct chain
    //-----------------------------------------------------------------
    config_data_modules(dataModules);

    pthread_mutex_unlock(&rpiNode.lock);
    //-----------------------------------------------------------
    // Free memory & complete
    //-----------------------------------------------------------
    json_decref(root);

    config_file_completed(pid);

    return 1;

}