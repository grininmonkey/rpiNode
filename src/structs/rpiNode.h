#ifndef RPINODE_H
#define RPINODE_H

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
#define RPI_MAX_TAGS 500                // Max number of Data Tags
#define RPI_MAX_CMD_LENGTH 500
#define RPI_MAX_META_ID 25
#define RPI_MAX_SINGLES 10              // Max number of locals SingleValues
#define RPI_MAX_META_DESC 255
#define RPI_MAX_META_TYPE 25
#define RPI_MAX_VALUE_LENGTH 255        // Max number of value characters
#define RPI_MAX_MULTI_VALUES 10
#define RPI_MAX_META_TIMESTAMP 22
#define RPI_MAX_CONFIG_LINE_LENGTH 256
// Sensor Specific
#define RPI_MAX_DS18B20S 10             // Max number of DS18B20 devcies

//---------------------------------------------------------------
//  Mostly use Fixed structs for speed, cache and simplicity
//---------------------------------------------------------------

//---------------------------------------------------------------
// Base (Tag) DataPoint
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
	SingleValue values[RPI_MAX_MULTI_VALUES];
} MultiValue;

//---------------------------------------------------------------
// Internal / Sensor specific
//---------------------------------------------------------------

typedef struct {
    int DS18B20_count;
    MultiValue MPU6050;
    MultiValue DS18B20[RPI_MAX_DS18B20S];
} Internal;

//---------------------------------------------------------------
//  External / Commands 
//---------------------------------------------------------------

typedef struct {
    int CMDS_count;
    SingleValue CMDS[RPI_MAX_SINGLES];
} External;

//---------------------------------------------------------------
//  Final Structure
//---------------------------------------------------------------

typedef struct {
    int tags_count;
    Config config;
    Internal internal;
    External external;
    SingleValue tags[RPI_MAX_TAGS];
    ServiceInfo *service_list;
    pthread_mutex_t lock;
} SharedData;

//---------------------------------------------------------------
//  Share
//---------------------------------------------------------------

extern SharedData rpiNode;

#endif 
