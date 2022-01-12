/*!
 * @file expand.c
 * @brief fichier de la partie expand
 * @author Pierre S.
 * @date 2021 - 2022
 */


#include "expand.h"
#include "table.h"

/*!
 * @brief expand extends a query according to its type (SELECT or INSERT)
 * @see expand_select
 * @see expand_insert
 * @param query a pointer to the select or insert query
 */
void expand(query_result_t *query) {
    if (query->query_type == QUERY_SELECT) {
        expand_select(&query->query_content.select_query);
    } else if (query->query_type == QUERY_INSERT) {
        expand_insert(&query->query_content.insert_query);
    }
}

/*!
 * @brief expand_select extends a select query (replaces the '*' with the list of fields in the target table definition)
 * Also uses get_table_definition from table.h
 * @param query a pointer to the select query
 */
void expand_select(update_or_select_query_t *query) {
    if (strcmp("*", query->set_clause.fields[0].column_name) == 0) {
        table_definition_t *table_definition = malloc(sizeof(table_definition_t));
        table_definition = get_table_definition(query->table_name, table_definition);
        for (int i=0; i<table_definition->fields_count; i++) {
            strcpy(query->set_clause.fields[i].column_name, table_definition->definitions[i].column_name); 
            
        }
        query->set_clause.fields_count = table_definition->fields_count;
    }
}

/*!
 * @brief expand_insert extends a insert query (adds to the query the fields of the table that are not defined there with their default value)
 * Also uses get_table_definition from table.h
 * @see is_field_in_record 
 * @see make_default_value 
 * @param query a pointer to the insert query
 */
void expand_insert(insert_query_t *query) {
    table_definition_t *table_definition = malloc(sizeof(table_definition_t));
    table_definition = get_table_definition(query->table_name,table_definition);
    for (int i=0; i<table_definition->fields_count; i++) {
        if (is_field_in_record(&query->fields_names, table_definition->definitions[i].column_name) == false) {
          strcpy(query->fields_names.fields[query->fields_names.fields_count].column_name, table_definition->definitions[i].column_name);
          query->fields_names.fields[query->fields_names.fields_count].field_type = table_definition->definitions[i].column_type;
          make_default_value(&query->fields_names.fields[query->fields_names.fields_count], query->table_name);
          query->fields_names.fields_count++;
        }
    }
    free(table_definition);
}



/*!
 * @brief is_field_in_record tests for the existence of a field of the name given in parameter in the table record of the query
 * @param record a pointer to the table record of the query
 * @param field_name the name of the field
 * @return true if it exists, false otherwise 
 */
bool is_field_in_record(table_record_t *record, char *field_name) {
  for (int i=0; i<record->fields_count; i++) {
    if (strcmp(record->fields[i].column_name, field_name) == 0) {
      return true;
    }
  }
  return false;
}

/*!
 * @brief make_default_value assigns the default value to a field based on its type
 * Also uses get_next_key from table.h
 * @param field the field to be assigned the default value
 * @param table_name the table name of the field
 */
void make_default_value(field_record_t *field, char *table_name) {
  switch (field->field_type) {
    case TYPE_PRIMARY_KEY:
        field->field_value.primary_key_value = get_next_key(field->column_name);
        break;
    case TYPE_INTEGER:
        field->field_value.int_value = 0;
        break;
    case TYPE_FLOAT:
        field->field_value.float_value = 0;
        break;
    case TYPE_TEXT:
        strcpy(field->field_value.text_value, "\0");
        break;
    default:
        break;
  }
}
