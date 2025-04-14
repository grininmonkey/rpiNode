#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3.h>
#include "../../structs/rpiNode.h"

int initializeDB(pid_t pid) {
    sqlite3 *db;
    char *errMsg = 0;
    char buffer[500];
    int rc;

    // Open (or create) the database 
    pthread_mutex_lock(&rpiNode.lock);
    rpiNode.config.DBcreated = 0;

    if (rpiNode.config.saveToDB != 1) {
        pthread_mutex_unlock(&rpiNode.lock);
        return 1;
    }

    if (rpiNode.config.useTmpfs != 1)
        if (access(rpiNode.config.currentValuesDB, F_OK) == 0)
            remove(rpiNode.config.currentValuesDB);
            
    snprintf(buffer, sizeof(buffer), "%s", rpiNode.config.currentValuesDB);
    rc = sqlite3_open(buffer, &db);
    if (rc) {
        fprintf(stderr, "[main][%d]: Can't open database: %s\n", pid, sqlite3_errmsg(db));
        return 0;
    } else {
        rpiNode.config.DBcreated = 1;
        printf("[main][%d]: Opened/Created DB: %s\n", pid, buffer);
    }
    
    pthread_mutex_unlock(&rpiNode.lock);

    // SQL statements
    const char *sql =
        "CREATE TABLE IF NOT EXISTS tblValues ("
        "id TEXT PRIMARY KEY,"
        "description TEXT,"
        "sensorType TEXT,"
        "updatedUTC TEXT,"
        "valueText TEXT,"
        "valueFloat FLOAT,"
        "valueTimeStamp TEXT"
        ");"

        "CREATE VIEW IF NOT EXISTS vValues AS "
        "SELECT * FROM tblValues;"

        "CREATE TRIGGER IF NOT EXISTS upsert_values "
        "INSTEAD OF INSERT ON vValues "
        "FOR EACH ROW "
        "BEGIN "
        "    UPDATE tblValues "
        "    SET "
        "        [description] = NEW.description,"
        "        sensorType = NEW.sensorType,"
        "        updatedUTC = CURRENT_TIMESTAMP,"
        "        valueText = NEW.valueText,"
        "        valueFloat = NEW.valueFloat,"
        "        valueTimeStamp = NEW.valueTimeStamp"
        "    WHERE id = NEW.id;"
        " "
        "    INSERT INTO tblValues (id, [description], sensorType, updatedUTC, valueText, valueFloat, valueTimeStamp) "
        "    SELECT NEW.id, NEW.description, NEW.sensorType, CURRENT_TIMESTAMP, NEW.valueText, NEW.valueFloat, NEW.valueTimeStamp "
        "    WHERE NOT EXISTS ("
        "        SELECT 1 FROM tblValues WHERE id = NEW.id"
        "    );"
        " "
        "END;";

    // Execute SQL
    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[main][%d]: SQL error: %s\n", pid, errMsg);
        sqlite3_free(errMsg);
        pthread_mutex_lock(&rpiNode.lock);
        rpiNode.config.DBcreated = 0;
        pthread_mutex_unlock(&rpiNode.lock);
        return 0;
    } else {
        printf("[main][%d]: Table, view, and trigger created successfully\n", pid);
    }

    // Close the database
    sqlite3_close(db);

    return 1;

}

