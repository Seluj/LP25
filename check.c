//
// Created by flassabe on 19/11/2021.
//

#include "check.h"

#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <dirent.h>

#include "table.h"

/*!
 * @brief function check_query is the high level check function, which will call specialized check functions and
 * return their result.
 * @param query a pointer to a query result structure returned after parsing the SQL query (@see sql.h)
 * @return true if the query is valid, false else
 */
bool check_query(query_result_t *query) {
    switch (query->query_type) {
        case QUERY_NONE:
            return false;
            break;
        case QUERY_CREATE_TABLE:
            return check_query_create(&query->query_content.create_query);
            break;
        case QUERY_DROP_TABLE:
            return check_query_drop_table(query->query_content.table_name);
            break;
        case QUERY_SELECT:
            return check_query_select(&query->query_content.select_query);
            break;
        case QUERY_UPDATE:
            return check_query_update(&query->query_content.update_query);
            break;
        case QUERY_DELETE:
            return check_query_delete(&query->query_content.delete_query);
            break;
        case QUERY_INSERT:
            return check_query_insert(&query->query_content.insert_query);
            break;
        case QUERY_DROP_DB:
            return check_query_drop_db(query->query_content.database_name);
            break;
        default:
            printf("Unsupported query code\n");
    }
    return false;
}

/*!
 * @brief check_query_select checks a select query
 * It relies on check_fields_list (list of fields to display and WHERE clause) and check_value_types (WHERE clause)
 * Also uses get_table_definition from table.h
 * @see check_fields_list
 * @see check_value_types
 * @see get_table_definition
 * @param query a pointer to the select query
 * @return true if valid, false if invalid
 */
bool check_query_select(update_or_select_query_t *query) {


    bool ret = false;
    if (table_exists(query->table_name) == 0) {
        printf("La table n'existe pas\n");
        ret = false;
    } else {
        table_definition_t *definition = malloc(sizeof(table_definition_t));
        definition = get_table_definition(query->table_name, definition);
        if (query->set_clause.fields_count == 1 && strcmp(query->set_clause.fields[0].field_value.text_value, "*") == 0) {
            if (query->where_clause.values.fields_count == 0) {
                ret = true;
            } else {
                if (check_value_types(&query->where_clause.values, definition)) {
                    ret = true;
                } else {
                    ret = false;
                }
            }
        } else {
            if (query->where_clause.values.fields_count == 0) {
                if (check_fields_list(&query->set_clause, definition)) {
                    ret = true;
                } else {
                    ret = false;
                }
            } else {
                if (check_value_types(&query->where_clause.values, definition) && check_fields_list(&query->set_clause, definition)) {
                    ret = true;
                } else {
                    ret = false;
                }
            }
        }
/*
        if (query->where_clause.values.fields_count == 0 && query->set_clause.fields_count == 0) {
            ret = true;
        } else {
            if (query->where_clause.values.fields_count != 0 && query->set_clause.fields_count == 0) {
                
            } else if (query->where_clause.values.fields_count == 0 && query->set_clause.fields_count != 0) {
                if (check_fields_list(&query->set_clause, definition)) {
                    ret = true;
                } else {
                    ret = false;
                }
            } else {
                if (check_value_types(&query->where_clause.values, definition) && check_fields_list(&query->set_clause, definition)) {
                    ret = true;
                } else {
                    ret = false;
                }
            }
        }*/
        free(definition);
    }
    return ret;
}

/*!
 * @brief check_query_update checks an update query
 * It relies on check_value_types and get_table_definition from table.h.
 * @see check_value_types
 * @see get_table_definition
 * @param query a pointer to the update query
 * @return true if valid, false if invalid
 */
bool check_query_update(update_or_select_query_t *query) {
    bool ret = false;
    if (table_exists(query->table_name) == 0) {
        printf("La table n'existe pas\n");
        ret = false;
    } else {
        table_definition_t *definition = malloc(sizeof(table_definition_t));
        definition = get_table_definition(query->table_name, definition);
        if (query->set_clause.fields_count == 0) {
            if (query->where_clause.values.fields_count == 0) {
                ret = true;
            } else {
                if (check_value_types(&query->where_clause.values, definition)) {
                    ret = true;
                } else {
                    ret = false;
                }
            }
        } else {
            if (query->where_clause.values.fields_count == 0) {
                if (check_value_types(&query->set_clause, definition)) {
                    ret = true;
                } else {
                    ret = false;
                }
            } else {
                if (check_value_types(&query->where_clause.values, definition) && check_value_types(&query->set_clause, definition)) {
                    ret = true;
                } else {
                    ret = false;
                }
            }
        }
        free(definition);
    }
    return ret;
}

/*!
 * @brief check_query_create checks a create query. It checks if the table doesn't already exist
 * @param query a pointer to the create query
 * @return true if valid, false if invalid
 */
bool check_query_create(create_query_t *query) {
    if (table_exists(query->table_name) == 1) {
        printf("La table existe deja\n");
        return false;
    } else {
        bool ret = true;
        int i = 0;
        while (i < query->table_definition.fields_count && ret) {
            switch (query->table_definition.definitions[i].column_type) {
                case TYPE_UNKNOWN:
                    printf("TYPE_UNKNOWN trouve\n");
                    ret = false;
                    break;
                default:
                    ret = true;
                    break;
            }
            i++;
        }
        return ret;
    }
}

/*!
 * @brief check_query_insert checks an insert query
 * It relies on check_value_types and get_table_definition from table.h.
 * Prior to checking the field/value pairs, you must join fields and values (extracted to different table_record_t
 * structs during the SQL parsing) by copying the values strings into the text_value member of the fields names
 * table_record_t structure.
 * @see check_value_types
 * @see get_table_definition
 * @param query a pointer to the insert query
 * @return true if valid, false if invalid
 */
bool check_query_insert(insert_query_t *query) {
    bool ret = false;
    if (table_exists(query->table_name) == 0) {
        printf("La table n'existe pas\n");
        ret = false;
    } else {
        table_definition_t *definition = malloc(sizeof(table_definition_t));
        definition = get_table_definition(query->table_name, definition);
        for (int w=0; w<query->fields_names.fields_count; w++) {
            strcpy(query->fields_names.fields[w].column_name, query->fields_names.fields[w].field_value.text_value);
        }

/*
        printf("\n================ TEST ================\n\n");
        printf("Table name :%s\n", query->table_name);
        printf("Il y a %d champs\n", query->fields_names.fields_count);
        printf("Il y a %d valeurs\n", query->fields_values.fields_count);
        int untrucaupif=0;
        printf("Champs:\n");
        while (untrucaupif< query->fields_names.fields_count) {
            printf("\t%d :%s\n", untrucaupif, query->fields_names.fields[untrucaupif].column_name);
            untrucaupif++;
        }
        untrucaupif=0;
        printf("Valeurs:\n");
        while (untrucaupif< query->fields_values.fields_count) {
            printf("\t%d :%s->%d\n", untrucaupif, query->fields_values.fields[untrucaupif].field_value.text_value, query->fields_values.fields[untrucaupif].field_type);
            untrucaupif++;
        }
        printf("\nTable Definitions:\n");
        printf("Il y a %d champs\n", definition->fields_count);
        untrucaupif=0;
        printf("Les Champs (nom->type):\n");
        while (untrucaupif<definition->fields_count) {
            printf("\t%s->%d\n", definition->definitions[untrucaupif].column_name, definition->definitions[untrucaupif].column_type);
            untrucaupif++;
        }
        printf("\n============== FIN TEST ==============\n\n");
*/

        if (query->fields_names.fields_count == 1 && strcmp("*", query->fields_names.fields[0].column_name) == 0) {
            ret = true;
        } else {
            if (query->fields_values.fields_count != query->fields_names.fields_count) {
                printf("Le nombre de champs ne correspond pas au nombre de valeurs entrees\n");
                ret = false;
            } else {
                ret = true;
            }
        }
        if (ret == true) {
            for (int i=0; i<query->fields_values.fields_count; i++) {
                strcpy(query->fields_names.fields[i].field_value.text_value, query->fields_values.fields[i].field_value.text_value);
            }
            query->fields_names.fields_count = query->fields_values.fields_count;
            
            /*
            printf("\n================ TEST ================\n\n");
            printf("Table name :%s\n", query->table_name);
            printf("Il y a %d champs\n", query->fields_names.fields_count);
            printf("Il y a %d valeurs\n", query->fields_values.fields_count);
            int untrucaupif=0;
            printf("Champs->valeurs:\n");
            while (untrucaupif< query->fields_names.fields_count) {
                printf("\t%d :%s->%s->%d\n", untrucaupif, query->fields_names.fields[untrucaupif].column_name, query->fields_names.fields[untrucaupif].field_value.text_value, query->fields_names.fields[untrucaupif].field_type);
                untrucaupif++;
            }
            printf("\n============== FIN TEST ==============\n\n");

            */

            if (check_value_types(&query->fields_names, definition)) {
                ret = true;

                /*
                printf("\n================ TEST ================\n\n");
                printf("Table name :%s\n", query->table_name);
                printf("Il y a %d champs\n", query->fields_names.fields_count);
                printf("Il y a %d valeurs\n", query->fields_values.fields_count);
                int untrucaupif=0;
                printf("Champs->type->valeurs:\n");
                while (untrucaupif< query->fields_names.fields_count) {
                    printf("\t%d :%s->%d->", untrucaupif, query->fields_names.fields[untrucaupif].column_name, query->fields_names.fields[untrucaupif].field_type);
                    switch (query->fields_names.fields[untrucaupif].field_type) {
                        case 1:
                            printf("%d\n", query->fields_names.fields[untrucaupif].field_value.primary_key_value);
                            break;
                        case 2:
                            printf("%d\n", query->fields_names.fields[untrucaupif].field_value.int_value);
                            break;
                        case 3:
                            printf("%f\n", query->fields_names.fields[untrucaupif].field_value.float_value);
                            break;
                        default:
                            printf("%s\n", query->fields_names.fields[untrucaupif].field_value.text_value);
                            break;
                    }
                    untrucaupif++;
                }
                printf("\n============== FIN TEST ==============\n\n");
                */

            } else {
                printf("Les valeurs entrees ne correspondent pas au type des colonnes\n");
                ret = false;
            }
        }
        free(definition);
    }
    return ret;
}

/*!
 * @brief check_query_delete checks a delete query
 * It relies on check_value_types (WHERE clause) and get_table_definition from table.h.
 * @see check_value_types
 * @see get_table_definition
 * @param query a pointer to the delete query
 * @return true if valid, false if invalid
 */
bool check_query_delete(delete_query_t *query) {
    bool ret = false;
    if (table_exists(query->table_name) == 0) {
        printf("La table n'existe pas\n");
        ret = false;
    } else {
        if (query->where_clause.values.fields_count == 0) {
            ret = true;
        } else {
            table_definition_t *definition = malloc(sizeof(table_definition_t));
            definition = get_table_definition(query->table_name, definition);
            if (check_value_types(&query->where_clause.values, definition)) {
                ret = true;
            } else {
                ret = false;
            }
        }
    }
    return ret;
}

/*!
 * @brief check_query_drop_table checks a drop table query: checks that the table exists
 * @param table_name the table name
 * @return true if valid, false if invalid
 */
bool check_query_drop_table(char *table_name) {
    if (table_exists(table_name) == 1) {
        return true;
    } else {
        printf("La table n'existe pas\n");
        return false;
    }
}

/*!
 * @brief check_query_drop_db checks a drop database query: checks that the database exists
 * @param db_name the database name
 * @return true if valid, false if invalid
 */
bool check_query_drop_db(char *db_name) {
    bool ret = false;
    chdir("..");
    char *tmp = malloc(sizeof(char) * strlen(db_name));
    strcpy(tmp, db_name);
    DIR *database;
    struct dirent *dirent;
    database = opendir(db_name);
    if (database == NULL) {
        printf("La base de données n'existe pas\n");
        ret = false;
    } else {
        dirent = readdir(database);
        int i=0;
        while (dirent != NULL) {
            i++;
            dirent = readdir(database);
        }
        closedir(database);
        if (i == 0) {
            printf("Il n'y a rien dans cette base de données\n");
            ret = false;
        } else {
            ret = true;
        }
    }
    free(tmp);
    chdir(db_name);
    return ret;
}

/*!
 * @brief function check_fields_list checks if all fields from a fields list exist in a table definition
 * Uses find_field_definition
 * @see find_field_definition
 * @param fields_list list of the fields to check
 * @param table_definition table definition within which to check
 * @return true if all fields belong to table, false else
 */
bool check_fields_list(table_record_t *fields_list, table_definition_t *table_definition) {
    int i = 0;
    bool ret = true;
    while (i < fields_list->fields_count && ret == true) {
        if (find_field_definition(fields_list->fields[i].column_name, table_definition) == NULL) {
            ret = false;
        } else {
            i++;
        }
    }
    return ret;
}

/*!
 * @brief function check_value_types checks if all fields in a record list belong to a table (through its definition)
 * AND if the corresponding value can be converted to the field type as specified in the table definition.
 * Uses find_field_definition and is_value_valid
 * @see find_field_definition
 * @see is_value_valid
 * @param fields_list list of the fields and their values (as text)
 * @param table_definition table definition
 * @return true if all fields belong to table and their value types are correct, false else
 */
bool check_value_types(table_record_t *fields_list, table_definition_t *table_definition) {
    field_definition_t *tmp;
    int i = 0;
    bool ret = true;
    while (i < fields_list->fields_count && ret == true) {
        tmp = find_field_definition(fields_list->fields[i].column_name, table_definition);
        if (tmp == NULL) {
            ret = false;
        } else {
            if (is_value_valid(&fields_list->fields[i], tmp) == true) {
                i++;
            } else {
                ret = false;
            }
        }
    }
    return ret;
}

/*!
 * @brief function find_field_definition looks up for a field name in a table definition.
 * @param field_name the field name to lookup for.
 * @param table_definition the table definition in which to search.
 * @return a pointer to the field definition structure if the field name exists, NULL if it doesn't.
 */
field_definition_t *find_field_definition(char *field_name, table_definition_t *table_definition) {
    int i = 0;
    bool trouve = false;
    while (i < table_definition->fields_count && trouve == false) {
        if (strcmp(table_definition->definitions[i].column_name, field_name) == 0) {
            trouve = true;
        } else {
            i++;
        }
    }
    if (trouve == true) {
        return &table_definition->definitions[i];
    } else {
        return NULL;
    }
}

/*!
 * @brief function is_value_valid checks if a field_record_t holding a field name and a text value matches a field definition:
 * it checks fields names, and tests if the text value can be converted into the table defined type.
 * /!\ If it succeeded, this function must modify the value in the field_record_t structure by converting and setting
 * the value to the proper type!
 * Uses is_int, is_float, is_key
 * @see is_int
 * @see is_float
 * @see is_key
 * @param value the field and value record
 * @param field_definition the field definition to test against
 * @return true if valid (and converted), false if invalid
 */
bool is_value_valid(field_record_t *value, field_definition_t *field_definition) {
    bool ret = false;
    if (strcmp(field_definition->column_name, value->column_name) == 0){
        switch (field_definition->column_type) {
            case TYPE_PRIMARY_KEY:
                if (is_key(value->field_value.text_value)) {
                    ret = true;
                    value->field_value.primary_key_value = strtoull(value->field_value.text_value, NULL, 10);
                    value->field_type = TYPE_PRIMARY_KEY;
                } else {
                    ret = false;
                }
                break;
            case TYPE_INTEGER:
                if (is_int(value->field_value.text_value)) {
                    ret = true;
                    value->field_value.int_value = strtoll(value->field_value.text_value, NULL, 10);
                    value->field_type = TYPE_INTEGER;
                } else {
                    ret = false;
                }
                break;
            case TYPE_FLOAT:
                if (is_float(value->field_value.text_value)) {
                    ret = true;
                    value->field_value.float_value = strtod(value->field_value.text_value, NULL);
                    value->field_type = TYPE_FLOAT;
                } else {
                    ret = false;
                }
                break;
            case TYPE_TEXT:
                ret = true;
                value->field_type = TYPE_TEXT;
                break;
            case TYPE_UNKNOWN:
                ret = false;
                value->field_type = TYPE_UNKNOWN;
                break;
        }
    } 
    return ret;
}

/*!
 * @brief function is_int tests if the string value is a text representation of an integer value.
 * You may use strtoll for this test.
 * @param value the text representation to test
 * @return true if value can be converted into an integer, false if it cannot
 */
bool is_int(char *value) {
    char *reste = NULL;
    strtoll(value, &reste, 10);
    if (strcmp(reste, "\0") != 0) {
        return false;
    } else {
        return true;
    }
}

/*!
 * @brief function is_float tests if the string value is a text representation of a double value.
 * You may use strtod for this test.
 * @param value the text representation to test
 * @return true if value can be converted into a double, false if it cannot
 */
bool is_float(char *value) {
    char *reste = NULL;
    strtod(value, &reste);
    if (strcmp(reste, "\0") != 0) {
        return false;
    } else {
        return true;
    }
}

/*!
 * @brief function is_int tests if the string value is a text representation of a key value.
 * You may use strtoull for this test.
 * @param value the text representation to test
 * @return true if value can be converted into a key, false if it cannot
 */
bool is_key(char *value) {
    bool ret = false;
    int tmp;
    char *reste = NULL;
    tmp = strtoull(value, &reste, 10);
    if (strcmp(reste, "\0") != 0) {
        ret = false;
    } else {
        if (tmp < 0) {
            ret = false;
        } else {
            ret = true;
        }
    }
    return ret;
}
