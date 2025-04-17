#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <jansson.h>
#include "readConfigFile.h"
#include "../structs/rpiNode.h"
#include "../utils/nameValue.h"

int test_string(const char *name, json_t *obj, int MAX) {
    if (json_is_string(obj)) {
        if (INT_GT_ZERO_AND_LE(strlen(json_string_value(obj)), MAX)) {
            return 1;
        } else {
            fprintf(stderr, 
                "[main][%d]: config <%s> length (%i) > max length (%i)\n",
                getpid(), name, strlen(json_string_value(obj)), MAX
            );
        }
    }
    return 0;
}

const char* get_string(const char *name, const char *current, json_t *new, int MAX) {
    if (test_string(name, new, MAX))
        return json_string_value(new);
    return current;
}

int get_true_false(int current, json_t *obj) {
    if (json_is_integer(obj))
        return INT_TRUE_FALSE(json_integer_value(obj));
    return current;
}

int get_integer(int current, json_t *obj) {
    if (json_is_integer(obj))
        return json_integer_value(obj);
    return current;
}

int read_config_file(pid_t pid) {
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
    json_t *internal = json_object_get(root, "internal");
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
    // TODO: dataModules pre populating rpiNode.data with 
    // DataModule Struct chain
    //-----------------------------------------------------------------
    if (json_is_array(dataModules)) {
        size_t module_index, component_index, nv_index;
        json_t  *element, *sub_element, *nv_element, *m_name, *c_name, *s_name,
                *value, *start, *verbose, *uniqueId, *components, *settings;
        json_array_foreach(dataModules, module_index, element) {
            m_name = json_object_get(element, "name");
            start = json_object_get(element, "start");
            verbose = json_object_get(element, "verbose");
            uniqueId = json_object_get(element, "uniqueId");
            components = json_object_get(element, "components");
            // Add/Set Module entry into array
            // ...
            if (json_is_array(components)) {
                json_array_foreach(components, component_index, sub_element) {
                    c_name = json_object_get(sub_element, "name");
                    settings = json_object_get(sub_element, "settings");
                    // Add/Set component entry into module[index].component[index]array
                    // ...
                    if (json_is_array(settings)) {
                        json_array_foreach(settings, nv_index, nv_element) {
                            s_name = json_object_get(nv_element, "name");
                            value = json_object_get(nv_element, "value");
                            // Add/set to settings pointer list
                            printf("data[%i].components[%i].settings[%i].name=%s\n", module_index, component_index, nv_index, json_string_value(s_name));
            
                        }
                    }
                }
            }
        }
    }
    pthread_mutex_unlock(&rpiNode.lock);
    //-----------------------------------------------------------
    // Free memory & complete
    //-----------------------------------------------------------
    
    json_decref(root);

    printf("[main][%d]: Config loaded successfully\n", pid);
    printf("[main][%d]:\t\t# id: %s\n", pid, rpiNode.config.id);
    printf("[main][%d]:\t\t# clusterID: %s\n", pid, rpiNode.config.clusterID);
    printf("[main][%d]:\t\t# http: %s\n", pid, INT_TRUE_FALSE_STR(rpiNode.config.http));
    printf("[main][%d]:\t\t# mDNS: %s\n", pid, INT_TRUE_FALSE_STR(rpiNode.config.mDNS));
    printf("[main][%d]:\t\t# master: %s\n", pid, INT_TRUE_FALSE_STR(rpiNode.config.master));
    printf("[main][%d]:\t\t# saveToDB: %s\n", pid, INT_TRUE_FALSE_STR(rpiNode.config.saveToDB));
    printf("[main][%d]:\t\t# useTmpfs: %s\n", pid, INT_TRUE_FALSE_STR(rpiNode.config.useTmpfs));
    printf("[main][%d]:\t\t# httpPort: %i\n", pid, rpiNode.config.httpPort);
    printf("[main][%d]:\t\t# broadcast: %s\n", pid, INT_TRUE_FALSE_STR(rpiNode.config.broadcast));
    printf("[main][%d]:\t\t# tmpfsSize: %i\n", pid, rpiNode.config.tmpfsSize);
    printf("[main][%d]:\t\t# broadcastIP: %s\n", pid, rpiNode.config.broadcastIP);
    printf("[main][%d]:\t\t# broadcastPort: %i\n", pid, rpiNode.config.broadcastPort);
    printf("[main][%d]:\t\t# tmpfsFolderName: %s\n", pid, rpiNode.config.tmpfsFolderName);
    printf("[main][%d]:\t\t# updateDBSeconds: %i\n", pid, rpiNode.config.updateDBSeconds);
    printf("[main][%d]:\t\t# currentValuesDB: %s\n", pid, rpiNode.config.currentValuesDB);
    printf("[main][%d]:\t\t# currentValuesDBName: %s\n", pid, rpiNode.config.currentValuesDBName);
   
    NameValue *current = rpiNode.internal_config;
    while (current != NULL) {
        printf("[main][%d]:\t\t# %s: %s\n", pid, current->name, current->value);
        current = current->next;
    }

    return 1;

}