//#############################################################
// Basic Template
// Make sure to keep .h, struct(s) and func unique to the chain
//#############################################################
#ifndef MODULE_1000_H // Make sure to set number e.g. MODULE_1043_H
#define MODULE_1000_H // Make sure to set number e.g. MODULE_1043_H
//-------------------------------------------------------------
// required include(s)
//-------------------------------------------------------------
#include "../../../structs/rpiNode.h"
#include "../../../utils/signalHandler.h"
//-------------------------------------------------------------
// Cache`ish Config Structure for the module
//-------------------------------------------------------------
typedef struct {
    char name[RPI_MAX_DATACOMPONENT_NAME]; // Required: Do not remove
    // Place named properties for Name/Value settings as defined in config.json
    // and make sure they are all type char ???[RPI_MAX_NAMEVALUE_NAME] 
    // the loading of component settings from config.json only expects string
    // Example:
    char mySetting1[RPI_MAX_NAMEVALUE_NAME];
    char mySetting2[RPI_MAX_NAMEVALUE_NAME];
} M1000_Component; // Make sure to set number e.g. M1043_Component

typedef struct {
    int start;                                              // Required: Do not remove
    int verbose;                                            // Required: Do not remove
    int components_count;                                   // Required: Do not remove
    int threadSleepMilliseconds;                            // Required: Do not remove
    char name[RPI_MAX_DATACONTAINER_NAME];                  // Required: Do not remove
    M1000_Component components[RPI_MAX_INTERNAL_OBJECTS];   // Required: Do not remove
} M1000_Config; // Make sure to set number e.g. M1043_Config
//-------------------------------------------------------------
// Init function for the module
//-------------------------------------------------------------
void* m_thread_1000(void* arg); // Make sure to set number e.g. m_thread_1043

#endif