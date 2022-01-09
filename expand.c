//
// Created by flassabe on 23/11/2021.
//

#include "expand.h"
#include "table.h"

void expand(query_result_t *query) {
    if (query->query_type == QUERY_SELECT) {
        expand_select(&query->query_content.select_query);
    } else if (query->query_type == QUERY_INSERT) {
        expand_insert(&query->query_content.insert_query);
    }
}

void expand_select(update_or_select_query_t *query) {
    if (strcmp("*", query->set_clause.fields[0].column_name) == 0) {
      table_definition_t *table_definition = malloc(sizeof(table_definition_t));
      table_definition = get_table_definition(query->table_name, table_definition);
      for (int i=0; i<table_definition->fields_count; i++) {
          strcpy(query->set_clause.fields[i].column_name, table_definition->definitions[i].column_name);      
      }
    free(table_definition);
    }

}

void expand_insert(insert_query_t *query) {
    table_definition_t *table_definition = malloc(sizeof(table_definition_t));
    table_definition = get_table_definition(query->table_name, table_definition);
    
        printf("\n================ TEST ================\n\n");
        printf("Table name :%s\n", query->table_name);
        printf("Il y a %d champs\n", query->fields_names.fields_count);
        int untrucaupif=0;
    
                printf("Champs->valeurs:\n");
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
        printf("\nTable Definitions:\n");
        printf("Il y a %d champs\n", table_definition->fields_count);
        untrucaupif=0;
        printf("Les Champs (nom->type):\n");
        while (untrucaupif<table_definition->fields_count) {
            printf("\t%s->%d\n", table_definition->definitions[untrucaupif].column_name, table_definition->definitions[untrucaupif].column_type);
            untrucaupif++;
        }
        printf("\n============== FIN TEST ==============\n\n");

    for (int i=0; i<table_definition->fields_count; i++) {
        if ((table_definition->definitions[i].column_type == TYPE_PRIMARY_KEY) || (strcmp(table_definition->definitions[i].column_name, query->fields_names.fields[i].column_name) == 0)) {
          field_record_t *field_record = malloc(sizeof(field_record_t));
          field_record = &query->fields_names.fields[i];
          make_default_value(field_record, query->table_name);
          free(field_record);
        }
    }
    free(table_definition);
}

bool is_field_in_record(table_record_t *record, char *field_name) {
  for (int i=0; i<record->fields_count; i++) {
    if (strcmp(record->fields[i].column_name, field_name) == 0) {
      return true;
    }
  }
  return false;
}

void make_default_value(field_record_t *field, char *table_name) {
  switch (field->field_type) {
    case TYPE_PRIMARY_KEY:
        field->field_value.primary_key_value++;
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
