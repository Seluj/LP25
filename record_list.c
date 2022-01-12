/*!
 * @file record_list.c
 * @brief fichier gestion record_list, utilisé lors des requêtes select 
 * @author Jules F. et Céliane A.
 * @date 2021 - 2022
 */

#include "record_list.h"

#include <stdio.h>
#include <string.h>

/*!
 * @brief function clears the record_list
 * @param list of records
 * @return void
 */
void clear_list(record_list_t *record_list) {
    if (record_list != NULL) {
        record_list_node_t *tmp = record_list->head;
        while (tmp) {
            record_list->head = tmp->next;
            free(tmp);
            tmp = record_list->head;
        }
        record_list->head = record_list->tail = NULL;
    }
}

/*!
 * @brief function adds a record to the list of records
 * @param list of records
 * @param record that will be added to the list
 * @return void
 */
void add_record(record_list_t *record_list, table_record_t *record) {
    if (!record_list)
        return;

    record_list_node_t *new_node = malloc(sizeof(record_list_node_t));
    memcpy(&new_node->record, record, sizeof (table_record_t));
    new_node->next = NULL;
    if (record_list->head == NULL) {
        record_list->head = record_list->tail = new_node;
        new_node->previous = NULL;
    } else {
        record_list->tail->next = new_node;
        new_node->previous = record_list->tail;
        record_list->tail = new_node;
    }
}

/*!
 * @brief function field_record_length returns a field display length, i.e. the characters count it requires to be
 * displayed (equal to number of digits, signs, and '.' for numbers, number of characters (excluding the trailing '\0')
 * for strings)
 * @param field_record the field value whose display length must be computed
 * @return the display length of the field
 */
int field_record_length(field_record_t *field_record) {
    long long tmp_int;
    double tmp_float;
    int nb_caractere = 0;
    bool boolean;
    bool trouver = false;
    FILE *file_temp;
    char *name_file_temp = {"file.temp"};
    char caractere[500] = {'\0'};

    switch (field_record->field_type) {
        case TYPE_PRIMARY_KEY:
            if (field_record->field_value.primary_key_value != 0) {
                nb_caractere = log10(field_record->field_value.primary_key_value) + 1;
            } else {
                nb_caractere = 1;
            }
            break;
        case TYPE_INTEGER:
            if (field_record->field_value.int_value != 0) {
                nb_caractere = log10(field_record->field_value.int_value) + 1;
                if (field_record->field_value.int_value < 0) {
                    nb_caractere++;
                }
            } else {
                nb_caractere = 1;
            }
            break;
        case TYPE_FLOAT:
            file_temp = fopen(name_file_temp, "w+");
            fprintf(file_temp, "%f", field_record->field_value.float_value);
            fseek(file_temp, 0, SEEK_SET);
            fscanf(file_temp, "%s", caractere);
            nb_caractere = strlen(caractere);
            fclose(file_temp);
            remove(name_file_temp);
            break;
        case TYPE_TEXT:
            nb_caractere = strlen(field_record->field_value.text_value);
            break;
        case TYPE_UNKNOWN:
            nb_caractere = strlen(field_record->column_name);
            break;
    }
    return nb_caractere;
}

/*!
 * @brief function display_table_record_list displays a select query result. It consists of 4 steps:
 * - Step 1: go through the whole result and compute the maximum size for each field (use an array of MAX_FIELDS_COUNT
 * integers). Also update max lengths with column_names values.
 * - Step 2: display result header: make a line formatted as +----+----+---+------+ etc. with '+' for columns limits,
 * a number of '-' equal to the maximum size of current field + 2, and so on.
 * - Step 2 bis: display the columns names, right-aligned, separated by char '|' and with a one space left-padding.
 * - Step 2 tes: add the same line as in step 2.
 * - Step 3: for each record in the result, display its values like you did with header column names.
 * Step 2 and 3 require that you add extra space padding to the left of the values for those to be aligned.
 * - Step 4: add a line as in step 2.
 * @param record_list the record list to display
 *
 * For instance, a record list with two fields named 'id' and 'label' and two records (1, 'blah'), and (2, 'foo') will
 * display:
  +----+-------+
  | id | label |
  +----+-------+
  |  1 |  blah |
  |  2 |   foo |
  +----+-------+
 */
void display_table_record_list(record_list_t *record_list) {
  //Step 1 going through the results to find max size
  int field_count = record_list->head->record.fields_count;
  int max_size[MAX_FIELDS_COUNT] = {0};
  record_list_node_t *temp = record_list->head;
  int tmp;
  //We go over every line
  while (temp != NULL) {
    //For each line we study every single field
    for (int i = 0; i < field_count; i++) {
      tmp = field_record_length(&temp->record.fields[i]);
      if (max_size[i] < tmp) {
        max_size[i] = tmp;
      }
    }
    temp = temp->next;
  }

  //Step 2 display field names with the proper format
  printf("\n");
  for (int i = 0; i< field_count; i++) {
    printf("+");
    for (int j = 0; j < max_size[i] + 2; j++) {
      printf("-");
    }
  }
  printf("+\n");
  temp = record_list->head;
  for (int i = 0; i< field_count; i++) {
    printf("| ");
    for (int j = 0; j < max_size[i] - strlen(temp->record.fields[i].column_name); j++) {
      printf(" ");
    }
    printf("%s", temp->record.fields[i].column_name);
    printf(" ");
  }
  printf("|\n");
  for (int i = 0; i< field_count; i++) {
    printf("+");
    for (int j = 0; j < max_size[i] + 2; j++) {
      printf("-");
    }
  }
  printf("+\n");

  //Step 3 display each value
  temp = temp->next;
  while (temp != NULL) {
    for (int i = 0; i < field_count; i++) {
      printf("| ");
      for (int j = 0; j < max_size[i] - field_record_length(&temp->record.fields[i]); j++) {
        printf(" ");
      }
      switch (temp->record.fields[i].field_type) {
        case TYPE_INTEGER:
          printf("%lld", temp->record.fields[i].field_value.int_value);
          break;
        case TYPE_FLOAT:
          printf("%f", temp->record.fields[i].field_value.float_value);
          break;
        case TYPE_TEXT:
          printf("%s", temp->record.fields[i].field_value.text_value);
          break;
        case TYPE_PRIMARY_KEY:
          printf("%llu", temp->record.fields[i].field_value.primary_key_value);
          break;
        case TYPE_UNKNOWN:
          printf("[display_table_record_list] Erreur avec le type d'une des valeurs.");
          break;
      }
      printf(" ");
    }
    printf("|\n");
    temp = temp->next;
  }

  //Step 4 display end line
  for (int i = 0; i< field_count; i++) {
    printf("+");
    for (int j = 0; j < max_size[i] + 2; j++) {
      printf("-");
    }
  }
  printf("+\n");
}

