#ifndef RPI_NODE_H
#define RPI_NODE_H
//---------------------------------------------------------------
// includes
//---------------------------------------------------------------
#include "config.h"
#include "serviceInfo.h"
#include <pthread.h>
//---------------------------------------------------------------
// macros
//---------------------------------------------------------------
#define INT_TRUE_FALSE(x) ((x) > 0 ? 1 : 0)
#define INT_TRUE_FALSE_STR(x) ((x) > 0 ? "true" : "false")
#define INT_GT_ZERO_AND_LE(x, y) (((x) > 0) && ((x) <= (y)) ? 1 : 0)
#define SAFE_STRCPY(dest, src)                              \
    do {                                                    \
        if ((const void *)(dest) != (const void *)(src)) {  \
            strncpy((dest), (src), sizeof(dest) - 1);       \
            (dest)[sizeof(dest) - 1] = '\0';                \
        }                                                   \
    } while(0)

// Base
#define RPI_CONFIG_FILE "/etc/rpiNode/config.json"
#define RPI_CONFIG_SERVE_PATH "/srv/rpiNode"
#define RPI_MAX_TAGS 1000               // Max number of Data Tags
#define RPI_MAX_CMD_LENGTH 500
#define RPI_MAX_META_ID 56
#define RPI_MAX_SINGLES 10              // Max number of locals SingleValues
#define RPI_MAX_META_DESC 255
#define RPI_MAX_META_TYPE 25
#define RPI_MAX_VALUE_LENGTH 255        // Max number of value characters
#define RPI_MAX_META_TIMESTAMP 22
#define RPI_MAX_CONFIG_LINE_LENGTH 256
#define RPI_MAX_INTERNAL_OBJECTS 10     // Max number of root internal objects
#define RPI_MAX_VALUES_PER_OBJECT 10
#define RPI_MAX_NAMEVALUE_NAME 256
#define RPI_MAX_NAMEVALUE_VALUE 256

#define RPI_MAX_DATACONTAINER_NAME 56
#define RPI_MAX_DATACOMPONENT_NAME 56
#define RPI_MAX_DATATAG_ID (RPI_MAX_DATACONTAINER_NAME + RPI_MAX_DATACOMPONENT_NAME + 58)
#define RPI_MAX_DATA 250
//---------------------------------------------------------------
// Base (Tag) DataPoint (Soon to be obsolete)
//---------------------------------------------------------------
typedef struct {
    char id[RPI_MAX_META_ID];
    char cmd[RPI_MAX_CMD_LENGTH];
    char type[RPI_MAX_META_TYPE];
    char timestamp[RPI_MAX_META_TIMESTAMP];
    char description[RPI_MAX_META_DESC];
} Meta;

typedef struct {
    Meta info;
    char value[RPI_MAX_VALUE_LENGTH];
} SingleValue;

typedef struct {
    int values_count;
	SingleValue values[RPI_MAX_VALUES_PER_OBJECT];
} MultiValue;
//---------------------------------------------------------------
//  Generic NameValue Pointer List
//---------------------------------------------------------------
typedef struct NameValue {
    char name[RPI_MAX_NAMEVALUE_NAME];
    char value[RPI_MAX_NAMEVALUE_VALUE];
    struct NameValue *next;
} NameValue;
//---------------------------------------------------------------
// Internal / Sensor specific
//---------------------------------------------------------------
typedef struct {
    int DS18B20_count;
    MultiValue MPU6050;
    MultiValue DS18B20[RPI_MAX_INTERNAL_OBJECTS];
} Internal;
//---------------------------------------------------------------
// More generic data structure  (TOBE)
//---------------------------------------------------------------
typedef struct {
    char Id[RPI_MAX_DATATAG_ID];
    char type[RPI_MAX_META_TYPE];
    char description[RPI_MAX_META_DESC];
    char timestamp[RPI_MAX_META_TIMESTAMP];
    char value[RPI_MAX_VALUE_LENGTH];
} DataTag;

typedef struct {
    int tags_count;
    char name[RPI_MAX_DATACOMPONENT_NAME];
    NameValue *settings;
	DataTag tags[RPI_MAX_VALUES_PER_OBJECT];
} DataComponent;

typedef struct {
    int flag;
    int start;
    int verbose;
    int uniqueId;
    int components_count;
    int threadSleepMilliseconds;
    char name[RPI_MAX_DATACONTAINER_NAME];
    DataComponent components[RPI_MAX_INTERNAL_OBJECTS];
} DataModule;
//---------------------------------------------------------------
//  Final Structure
//---------------------------------------------------------------
typedef struct {
    int tags_count;
    int data_count;
    Config config;
    NameValue *internal_config;
    Internal internal;
    SingleValue tags[RPI_MAX_TAGS];
    ServiceInfo *service_list;
    DataModule data[RPI_MAX_DATA];
    pthread_mutex_t lock;
} SharedData;
//---------------------------------------------------------------
//  Share
//---------------------------------------------------------------

extern SharedData rpiNode;

#endif 
