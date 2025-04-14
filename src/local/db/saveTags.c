
#include <stdio.h>
#include <sqlite3.h>
#include "../../structs/rpiNode.h"
#include "../../utils/updateTags.h"

int save_tags(pid_t t_pid) {
    //-----------------------------------------------------------------
    //Read and Set locals from config
    //-----------------------------------------------------------------
    pthread_mutex_lock(&rpiNode.lock);
    if (rpiNode.config.DBcreated != 1) {
        pthread_mutex_unlock(&rpiNode.lock);
        return 1;
    }
    char db_name[sizeof(rpiNode.config.currentValuesDB)], table_name[sizeof(rpiNode.config.currentValuesViewName)];
    snprintf(db_name, sizeof(db_name), "%s", rpiNode.config.currentValuesDB);
    snprintf(table_name, sizeof(table_name), "%s", rpiNode.config.currentValuesViewName);
    pthread_mutex_unlock(&rpiNode.lock);
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    if (update_tags() == 1) {
        sqlite3* db;
        sqlite3_stmt* stmt;
        //char* errmsg = 0;
        int rc;

        // Open database
        rc = sqlite3_open(db_name, &db);
        if (rc) {
            fprintf(stderr, "[DB][%d]: Can't open database: %s\n", t_pid, sqlite3_errmsg(db));
            return rc;
        }        

        // Prepare insert SQL
        char sql[500];
        snprintf(sql, sizeof(sql),
                "INSERT INTO %s (id, description, sensorType, valueText, valueTimeStamp) VALUES (?, ?, ?, ?, ?);",
                table_name);  
                
        // Prepare insert statement
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "[DB][%d]: Failed to prepare statement: %s\n", t_pid, sqlite3_errmsg(db));
            sqlite3_close(db);
            return 1;
        } 
        
        // Start transaction for better performance
        sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
        
        //------------------------------------------------------------
        pthread_mutex_lock(&rpiNode.lock);
        for (int i = 0; i < rpiNode.tags_count; i++) {
            if (rpiNode.tags[i].info.id[0] != '\0') {
                // Bind values
                sqlite3_bind_text(stmt, 4, rpiNode.tags[i].value, -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 1, rpiNode.tags[i].info.id, -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 3, rpiNode.tags[i].info.type, -1, SQLITE_STATIC);
                sqlite3_bind_text(stmt, 5, rpiNode.tags[i].info.timestamp, -1, SQLITE_STATIC);  
                sqlite3_bind_text(stmt, 2, rpiNode.tags[i].info.description, -1, SQLITE_STATIC);
                
                rc = sqlite3_step(stmt);
                if (rc != SQLITE_DONE) {
                    fprintf(stderr, "Insert Failed: %s\n", sqlite3_errmsg(db));
                }

                sqlite3_reset(stmt);
                sqlite3_clear_bindings(stmt);
            }
        }
        pthread_mutex_unlock(&rpiNode.lock);
        //------------------------------------------------------------        

        // Commit transaction
        sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);        

        // Finalize statement and close DB
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------

    return 1;
}