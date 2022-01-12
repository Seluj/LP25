/*!
 * @file query_exec.c
 * @brief fichier de la partie expand
 * @author Céliane A. et Jules F. (Edit : Quentin M.)
 * @date 2021 - 2022
 */

#include "query_exec.h"
#include "database.h"
#include "table.h"

#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>


/*!
 *@brief calls the proper function depending on the user's query
 *@param query structure
 */
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

/*!
 *@brief execute_create executes a query to create a table 
 *@see create_table from table.h
 *@param query the query to be executed
 */
void execute_create(create_query_t *query){
    create_table(query);
}

/*!
 * @brief execute_insert insert a record from a table 
 * @Also uses add_row_to_table from table.h
 * @param query the query to be executed
 */
void execute_insert(insert_query_t *query) {
    add_row_to_table(query->table_name, &query->fields_names);
}

/*!
 *@brief delete_field delete a field
 *@param def contains the different fields found in the table
 *@param record structure of the different values found in one row of the table
 *@param set_clause structure containing the field names that the user entered
 */
void delete_field(table_definition_t *def, table_record_t *record, table_record_t *set_clause) {
    bool trouver, bollean;
    int z;
    
    for (int x=0; x<def->fields_count; x++) {
        trouver=false;
        for (int y=0; y<set_clause->fields_count; y++) {
            if (strcmp(def->definitions[x].column_name, set_clause->fields[y].column_name) == 0) {
                trouver = true;
            }
        }
        if (trouver == false) {
            z = 0;
            bollean = false;
            while (z < record->fields_count && bollean == false) {
                if (strcmp(record->fields[z].column_name, def->definitions[x].column_name) == 0){
                    bollean = true;
                } else {
                    z++;
                }
            }
            record->fields_count--;
            for (int g=z; g<record->fields_count; g++) {
                record->fields[g].field_type=record->fields[g+1].field_type;
                strcpy(record->fields[g].column_name, record->fields[g+1].column_name);
                switch (record->fields[g].field_type) {
                    case TYPE_PRIMARY_KEY:
                        record->fields[g].field_value.primary_key_value = record->fields[g+1].field_value.primary_key_value;
                        break;
                    case TYPE_INTEGER:
                        record->fields[g].field_value.int_value = record->fields[g+1].field_value.int_value;
                        break;
                    case TYPE_FLOAT:
                        record->fields[g].field_value.float_value = record->fields[g+1].field_value.float_value;
                        break;
                    case TYPE_TEXT:
                        strcpy(record->fields[g].field_value.text_value, record->fields[g+1].field_value.text_value);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

/*!
 *@brief execute_select select a record from a table 
 *@param query the query to be executed
 */
void execute_select(update_or_select_query_t *query) {
    FILE *index_file = open_index_file(query->table_name, "rb");
    if (!index_file)
        return;
    uint8_t active;
    int nb = 36;
    int i = 0;
    table_definition_t *def = malloc(sizeof(table_definition_t));
    def = get_table_definition(query->table_name, def);
    uint16_t length = compute_record_length(def);
    table_record_t *record = malloc(sizeof(table_record_t));
    record_list_t *list = malloc(sizeof(record_list_t));
    bool trouver;
    bool bollean;
    int z;
    for (int i=0; i<def->fields_count; i++) {
        record->fields[i].field_type = TYPE_UNKNOWN;
        strcpy(record->fields[i].column_name, def->definitions[i].column_name);
    }

    record->fields_count = def->fields_count;
    

    if (query->where_clause.values.fields_count == 0) {
        if (strcmp(query->set_clause.fields[0].field_value.text_value, "*") == 0) {
            add_record(list, record);
            while (nb != 0) {
                nb = fread(&active, sizeof(uint8_t), 1, index_file);
                if (active == 1) {
                    record = get_table_record(query->table_name, i*length, def, record);
                    add_record(list, record);
                }
                fseek(index_file, sizeof(uint32_t) + sizeof(uint16_t), SEEK_CUR);
                active = 0;
                i++;
            }
        } else {
            delete_field(def, record, &query->set_clause);
            add_record(list, record);
            while (nb != 0) {
                nb = fread(&active, sizeof(uint8_t), 1, index_file);
                if (active == 1) {
                    record = get_table_record(query->table_name, i*length, def, record);
                    delete_field(def, record, &query->set_clause);
                    add_record(list, record);
                }
                fseek(index_file, sizeof(uint32_t) + sizeof(uint16_t), SEEK_CUR);
                active = 0;
                i++;
            }
        } 
    }
    display_table_record_list(list);
}

/*!
 *@brief execute_update update a record from a table 
 *@param query the query to be executed
 *@return 
 */
void execute_update(update_or_select_query_t *query) {
  FILE *data_file = open_content_file(query->table_name, "wb");
    if (query->where_clause.values.fields_count == 0) {
      
    } else{     
     
   }   
}


/*!
 *@brief execute_delete delete a record from a table
 *@param query the query to be executed
 */
void execute_delete(delete_query_t *query) {
  long long position;
  int numero_de_ligne;
    if (query->where_clause.values.fields_count == 0) { // No where clause: clear table
        FILE *data_file = open_content_file(query->table_name, "wb");
        if (data_file) {
            fclose(data_file);
        }
        FILE *index_file = open_index_file(query->table_name, "wb");
        if (index_file) {
            fclose(index_file);
        }
    } else {
      //...
    }
}

/*!
 * @brief execute_drop_table delete a table
 * @Also uses drop_table from table.h
 * @param table_name the name of the table to be deleted
 */
void execute_drop_table(char *table_name) {
    if (directory_exists(table_name)) {
        drop_table(table_name);
    }
}

/*!
 * @brief execute_drop_database delete a database
 * @Also uses recursive_rmdir from database.h
 * @param db_name the name of the database to be deleted
 */
void execute_drop_database(char *db_name) {
    chdir("..");
    recursive_rmdir(db_name);
    exit(EXIT_SUCCESS);
}
