#include <stdio.h>
#include "../structs/rpiNode.h"

void config_file_completed(pid_t pid) {

    if (rpiNode.verbose) {

        printf("[main][%d]: Config loaded successfully\n", pid);
        printf("[main][%d]:\t\t# id: %s\n", pid, rpiNode.config.id);
        printf("[main][%d]:\t\t# clusterID: %s\n", pid, rpiNode.config.clusterID);
        printf("[main][%d]:\t\t# http: %s\n", pid, INT_TRUE_FALSE_STR(rpiNode.config.http));
        printf("[main][%d]:\t\t# mDNS: %s\n", pid, INT_TRUE_FALSE_STR(rpiNode.config.mDNS));
        printf("[main][%d]:\t\t# master: %s\n", pid, INT_TRUE_FALSE_STR(rpiNode.config.master));
        printf("[main][%d]:\t\t# serveMQ: %s\n", pid, INT_TRUE_FALSE_STR(rpiNode.config.serveMQ));
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
        printf("[main][%d]:\t\t[DataModules]------------\n", pid);
        for (int m = 0; m < rpiNode.data_count; m++) {
            DataModule *dm = &rpiNode.data[m];
            printf("[main][%d]:\t\ttagPathName: %s\n", pid, dm->tagPathName);
            printf("[main][%d]:\t\tstart: %i\n", pid, dm->start);
            printf("[main][%d]:\t\tverbose: %i\n", pid, dm->verbose);
            printf("[main][%d]:\t\tmoduleId: %i\n", pid, dm->moduleId);
            printf("[main][%d]:\t\tthreadSleepMilliseconds: %i\n", pid, dm->threadSleepMilliseconds);
            printf("[main][%d]:\t\t\t[Components]....\n", pid);
            for (int c = 0; c < dm->components_count; c++) {
                DataComponent *dc = &rpiNode.data[m].components[c];
                printf("[main][%d]:\t\t\ttagPathName: %s\n", pid, dc->tagPathName);
                NameValue *setting = dc->settings;
                while (setting != NULL) {
                    printf("[main][%d]:\t\t\t%s: %s\n", pid, setting->name, setting->value);
                    setting = setting->next;
                }
                printf("[main][%d]:\t\t\t.................\n", pid);
            }
            printf("[main][%d]:\t\t-------------------------\n", pid);
        }
        //printf("[main][%d]:\t\t# data_count: %i\n", pid, rpiNode.data_count);

        NameValue *current = rpiNode.internal_config;
        while (current != NULL) {
            printf("[main][%d]:\t\t# %s: %s\n", pid, current->name, current->value);
            current = current->next;
        }
    
    }

}