#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_MAX_CLUSTER_ID 50
#define CONFIG_MAX_ID 255
#define CONFIG_MAX_DB 500
#define CONFIG_MAX_DB_NAME 25
#define CONFIG_MAX_BROADCASTIP 16
#define CONFIG_MAX_TMPFSFOLDERNAME 255

typedef struct {
    int http;
    int mDNS;
    int master;
    int MPU6050;
    int DS18B20;
    int useTmpfs;
    int saveToDB;
    int httpPort;
    int tmpfsSize;
    int DBcreated;
    int broadcast;
    int broadcastPort;
    int updateDBSeconds;
    int DS18B20scanSeconds;
    int MPU6050scanMilliseconds;
    char id[CONFIG_MAX_ID];
    char clusterID[CONFIG_MAX_CLUSTER_ID];
    char broadcastIP[CONFIG_MAX_BROADCASTIP];
    char tmpfsFolderName[CONFIG_MAX_TMPFSFOLDERNAME];
    char currentValuesDB[CONFIG_MAX_DB];
    char currentValuesDBName[CONFIG_MAX_DB_NAME];
    char currentValuesViewName[25];
} Config;

#endif

