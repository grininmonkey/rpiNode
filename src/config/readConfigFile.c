#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <jansson.h>
#include "readConfigFile.h"
#include "../structs/rpiNode.h"

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
    json_t *commands = json_object_get(root, "commands");
    json_t *useTmpfs = json_object_get(root, "useTmpfs");
    json_t *tmpfsSize = json_object_get(root, "tmpfsSize");
    json_t *broadcast = json_object_get(root, "broadcast");
    json_t *clusterID = json_object_get(root, "clusterID");
    json_t *broadcastIP = json_object_get(root, "broadcastIP");
    json_t *broadcastPort = json_object_get(root, "broadcastPort");
    json_t *tmpfsFolderName = json_object_get(root, "tmpfsFolderName");
    json_t *updateDBSeconds = json_object_get(root, "updateDBSeconds");
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
    // Internal Sensor flags
    //-----------------------------------------------------------------
    if (json_is_object(internal)) {
        json_t *MPU6050 = json_object_get(internal, "MPU6050");
        json_t *DS18B20 = json_object_get(internal, "DS18B20");
        json_t *DS18B20scanSeconds = json_object_get(internal, "DS18B20scanSeconds");
        json_t *MPU6050scanMilliseconds = json_object_get(internal, "MPU6050scanMilliseconds");
        rpiNode.config.MPU6050 = get_true_false(rpiNode.config.MPU6050, MPU6050);
        rpiNode.config.DS18B20 = get_true_false(rpiNode.config.DS18B20, DS18B20);
        rpiNode.config.DS18B20scanSeconds = get_integer(rpiNode.config.DS18B20scanSeconds, DS18B20scanSeconds);    
        rpiNode.config.MPU6050scanMilliseconds = get_integer(rpiNode.config.MPU6050scanMilliseconds, MPU6050scanMilliseconds);    
    }
    //-----------------------------------------------------------------
    // External commands
    //-----------------------------------------------------------------
    if (json_is_array(commands)) {
        size_t index;
        json_t *lang;
        json_array_foreach(commands, index, lang) {
            if (json_is_string(lang)) {
                printf("  - %s\n", json_string_value(lang));
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
    printf("[main][%d]:\t\t# MPU6050: %s\n", pid, INT_TRUE_FALSE_STR(rpiNode.config.MPU6050));
    printf("[main][%d]:\t\t# DS18B20: %s\n", pid, INT_TRUE_FALSE_STR(rpiNode.config.DS18B20));
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
    printf("[main][%d]:\t\t# DS18B20scanSeconds: %i\n", pid, rpiNode.config.DS18B20scanSeconds);
    printf("[main][%d]:\t\t# currentValuesDBName: %s\n", pid, rpiNode.config.currentValuesDBName);
    printf("[main][%d]:\t\t# MPU6050scanMilliseconds: %i\n", pid, rpiNode.config.MPU6050scanMilliseconds);

    return 1;

}