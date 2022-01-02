//
// Created by flassabe on 22/11/2021.
//

#include "query_exec.h"

#include <dirent.h>
#include <unistd.h>

#include "database.h"
#include "table.h"


void execute(query_result_t *query) {
    switch (query->query_type) {
        case QUERY_CREATE_TABLE:
            execute_create(&query->query_content.create_query);
            break;
        case QUERY_INSERT:
            execute_insert(&query->query_content.insert_query);
            break;
        case QUERY_DROP_TABLE:
            execute_drop_table(query->query_content.table_name);
            break;
        case QUERY_DROP_DB:
            execute_drop_database(query->query_content.database_name);
            break;
        case QUERY_SELECT:
            execute_select(&query->query_content.select_query);
            break;
        case QUERY_UPDATE:
            execute_update(&query->query_content.update_query);
            break;
        case QUERY_DELETE:
            execute_delete(&query->query_content.delete_query);
            break;
        case QUERY_NONE:
            break;
    }
}

void execute_create(create_query_t *query){
    create_table(query);
}


void execute_insert(insert_query_t *query) {
    add_row_to_table(query->table_name, &query->fields_names);
}

void execute_select(update_or_select_query_t *query) {}

void execute_update(update_or_select_query_t *query) {}

void execute_delete(delete_query_t *query) {
    if (query->where_clause.values.fields_count == 0) { // No where clause: clear table
        chdir(query->table_name);
        FILE *data_file = open_content_file(query->table_name, "wb");
        if (data_file) {
            fclose(data_file);
        }
        FILE *index_file = open_index_file(query->table_name, "wb");
        if (index_file) {
            fclose(index_file);
        }
        chdir("..");
    } else {
        //
    }
}

void execute_drop_table(char *table_name) {
    if (directory_exists(table_name)) {
        drop_table(table_name);
    }
}

void execute_drop_database(char *db_name) {
    chdir("..");
	recursive_rmdir(db_name);
}
