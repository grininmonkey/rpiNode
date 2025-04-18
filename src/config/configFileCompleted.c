#include <stdio.h>
#include "../structs/rpiNode.h"

void config_file_completed(pid_t pid) {

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

    printf("[main][%d]:\t\t# data_count: %i\n", pid, rpiNode.data_count);

    NameValue *current = rpiNode.internal_config;
    while (current != NULL) {
        printf("[main][%d]:\t\t# %s: %s\n", pid, current->name, current->value);
        current = current->next;
    }

}