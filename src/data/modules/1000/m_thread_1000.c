//#############################################################
// Basic Template
// Make sure to update 1000 to the correct number (everywhere)
// To avoid confusion, maintain new modular .c .h files
// with standard filename prefix, ifndef and func names.
// e.g. m_thread_1000_my_func.h m_thread_1000_my_func.c
//#############################################################
#include <stdio.h>
#include "m_thread_1000.h"
//-------------------------------------------------------------
// Update this function to properly set settings properties
//-------------------------------------------------------------
void update_config_if_needed(int uniqueId, M1000_Config *config) {
    pthread_mutex_lock(&rpiNode.lock);
    //if (rpiNode.data.settings.update_flag == 1) {
    //    *config = rpiNode.data.settings.config;
    //    rpiNode.data.settings.update_flag = 0;
    //}
    pthread_mutex_unlock(&rpiNode.lock);
}
//-------------------------------------------------------------
// Entry (main) thread function
//-------------------------------------------------------------
void* m_thread_1000(void* arg) {
    M1000_Config config;  //local config cache
    update_config_if_needed(1000, M1000_Config *config)


    
    return NULL;
}