/*!
 * @file table.c
 * @brief fichier de gestion de table
 * @author Céliane A. et Jules F. (Edit : Quentin M.)
 * @date 2021 - 2022
 */


#include "table.h"

#include <sys/stat.h>
#include <unistd.h>
#include <math.h>

/*!
 * @brief function open_definition_file opens the table key file
 * @param table_name the name of the table whose definition file must be opened
 * @param mode the opening mode (as in fopen)
 * @return and pointer to a FILE type, resulting from the fopen function
 */
FILE *open_definition_file(char *table_name, char *mode) {
    char *file;
    int lenth = strlen(table_name);
    if (table_exists(table_name)) {
        chdir(table_name);
        file = malloc(sizeof(char) * (lenth + strlen(".def")));
        strcpy(file, table_name);
        strcat(file, ".def");
        FILE *definition_file;
        definition_file = fopen(file, mode);
        free(file);
        chdir("..");
        if (definition_file != NULL) {
            return definition_file;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

/*!
 * @brief function open_index_file opens the table key file
 * @param table_name the name of the table whose index file must be opened
 * @param mode the opening mode (as in fopen)
 * @return and pointer to a FILE type, resulting from the fopen function
 */
FILE *open_index_file(char *table_name, char *mode) {
    char *file;
    int lenth = strlen(table_name);
    if (table_exists(table_name)) {
        chdir(table_name);
        file = malloc(sizeof(char) * (lenth + strlen(".idx")));
        strcpy(file, table_name);
        strcat(file, ".idx");
        FILE *definition_file;
        definition_file = fopen(file, mode);
        free(file);
        chdir("..");
        if (definition_file != NULL) {
            return definition_file;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

/*!
 * @brief function open_content_file opens the table key file
 * @param table_name the name of the table whose content file must be opened
 * @param mode the opening mode (as in fopen)
 * @return and pointer to a FILE type, resulting from the fopen function
 */
FILE *open_content_file(char *table_name, char *mode) {
    char *file;
    int lenth = strlen(table_name);
    if (table_exists(table_name)) {
        chdir(table_name);
        file = malloc(sizeof(char) * (lenth + strlen(".data") + 1));
        strcpy(file, table_name);
        strcat(file, ".data");
        FILE *definition_file;
        definition_file = fopen(file, mode);
        free(file);
        chdir("..");
        if (definition_file != NULL) {
            return definition_file;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

/*!
 * @brief function open_key_file opens the table key file
 * @param table_name the name of the table whose key file must be opened
 * @param mode the opening mode (as in fopen)
 * @return and pointer to a FILE type, resulting from the fopen function
 */
FILE *open_key_file(char *table_name, char *mode) {
    char *file;
    int lenth = strlen(table_name);
    if (table_exists(table_name)==1) {
        chdir(table_name);
        file = malloc(sizeof(char) * (lenth + strlen(".key")+1));
        strcpy(file, table_name);
        strcat(file, ".key");
        FILE *definition_file;
        definition_file = fopen(file, mode);
        free(file);
        chdir("..");
        if (definition_file != NULL) {
            return definition_file;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

/*!
 * @brief function table_exists check if the table given exist
 * @param table_name the table to check
 * @return 1 if the table already exist, 0 if not
 */
int table_exists(const char table_name[]) {
    char *table = malloc(sizeof(char) * (strlen(table_name)+1));
    strcpy(table, table_name);
    if (directory_exists(table)) {
        return 1;
    } else {
        return 0;
    }
}

/*!
 * @brief function create_table creates the files for a new table. It checks if the table doesn't already exist,
 * creates its directory, its definition file, whose content is written based on the definition. It creates both
 * index and content empty files, and if there is a primary key, it creates the primary key file with a stored value
 * of 1.
 * @param table_definition a pointer to the definition of the new table
 */
void create_table(create_query_t *table_definition) {
    int i = 0;

    FILE *def_file;
    FILE *key_file;
    FILE *index_file;
    FILE *content_file;

    _mkdir(table_definition->table_name);

    def_file = open_definition_file(table_definition->table_name, "w+");
    index_file = open_index_file(table_definition->table_name, "w+");
    content_file = open_content_file(table_definition->table_name, "w+");
    if (def_file == NULL || index_file == NULL || content_file == NULL) {
        chdir("..");
        return;
    }

    fclose(index_file);
    fclose(content_file);
    
    while (i < table_definition->table_definition.fields_count) {
        if (table_definition->table_definition.definitions[i].column_type == TYPE_PRIMARY_KEY) {
            unsigned long long to_write = 1;
            key_file = open_key_file(table_definition->table_name, "wb+");
            fwrite(&to_write, sizeof(unsigned long long), 1, key_file);
            fclose(key_file);
        }
        fprintf(def_file, "%d %s\n", table_definition->table_definition.definitions[i].column_type, table_definition->table_definition.definitions[i].column_name);
        i++;
    }
    fclose(def_file);
    printf("La table %s a ete cree\n", table_definition->table_name);
}

/*!
 * @brief function drop_table removes all files and directory related to a table
 * @param table_name the name of the dropped table.
 */
void drop_table(char *table_name) {
    printf("la table %s va être supprimée\n", table_name);
    recursive_rmdir(table_name);
}

/*!
 * @brief function get_table_definition reads and returns a table definition
 * @param table_name the name of the target table
 * @param result a pointer to a previously allocated table_definition_t structure where the result will be stored.
 * @return the pointer to result, NULL if the function failed
 */
table_definition_t *get_table_definition(char *table_name, table_definition_t *result) {
    if (table_exists(table_name)) {
        FILE *definition_file;
        definition_file = open_definition_file(table_name, "r");
        if (definition_file == NULL) {
            return NULL;
        } else {
            int type_tmp, fields_count = 0;
            char name_tmp[TEXT_LENGTH];
            while (fields_count < MAX_FIELDS_COUNT && fscanf(definition_file, "%d %s", &type_tmp, name_tmp) != EOF) {
                switch (type_tmp) {
                    case 0:
                        result->definitions[fields_count].column_type = TYPE_UNKNOWN;
                        return NULL;
                        break;
                    case 1:
                        result->definitions[fields_count].column_type = TYPE_PRIMARY_KEY;
                        break;
                    case 2:
                        result->definitions[fields_count].column_type = TYPE_INTEGER;
                        break;
                    case 3:
                        result->definitions[fields_count].column_type = TYPE_FLOAT;
                        break;
                    case 4:
                        result->definitions[fields_count].column_type = TYPE_TEXT;
                        break;
                    default:
                        return NULL;
                        break;
                }
                strcpy(result->definitions[fields_count].column_name, name_tmp);
                fields_count++;
            }
            if (fields_count == 0) {
                return NULL;
            } else {
                result->fields_count = fields_count;
                return result;
            }
        }
    } else {
        return NULL;
    }
}

/*!
 * @brief function compute_record_length computed the in-memory length of a table record. It sums all fields sizes
 * by their respective sizes. Text is always stored on TEXT_LENGTH chars so a text field in-memory size is always
 * TEXT_LENGTH, whatever its actual content strlen.
 * @param definition the table definition whose record length we need
 * @return the value of the record length.
 */
uint16_t compute_record_length(table_definition_t *definition) {
    uint16_t length = 0;
    for (int i=0; i<definition->fields_count; i++) {
        switch (definition->definitions[i].column_type) {
            case TYPE_TEXT:
                length += 150;
                break;
            case TYPE_INTEGER:
                length += sizeof(long long);
                break;
            case TYPE_FLOAT:
                length += sizeof(double);
                break;
            case TYPE_PRIMARY_KEY:
                length += sizeof(unsigned long long);
            default:
                break;
        }
    }
    return length;
}

/*!
 * @brief function find_first_free_record finds the first free record in the index file. The first free record is:
 * - the first index_record_t occurrence in the index file whose active field is 0. Its active field must be put to 1
 * before ending the function (replace value inplace)
 * - if none exists: a new field_record_t added to the end of the file, with active set to 1, and others fields to 0.
 * @param table_name the name of the target table
 * @return the offset of the free index in the index file.
 */
uint32_t find_first_free_record(char *table_name) {
    uint8_t active = 1;
    uint32_t offset = 0;
    uint16_t length = 0;
    uint8_t active_to_write = 1;
    int nombre = 2;
    FILE *index_file;
    index_file = open_index_file(table_name, "rb+");
    table_definition_t *def = malloc(sizeof(table_definition_t));
    def = get_table_definition(table_name, def);
    if (index_file != NULL) {
        while (active == 1 && nombre != 0) {
            nombre = fread(&active, sizeof(uint8_t), 1, index_file);
            if (active == 1 && nombre != 0) {
                fread(&offset, sizeof(uint32_t), 1, index_file);
                fread(&length, sizeof(uint16_t), 1, index_file);
            }
        }
        if (nombre == 0) {
            uint32_t offset_to_write = offset+length;
            fwrite(&active_to_write, sizeof(uint8_t), 1, index_file);
            fwrite(&offset_to_write, sizeof(uint32_t), 1, index_file);
            if (length == 0) {
              length = compute_record_length(def);
            }
            fwrite(&length, sizeof(uint16_t), 1, index_file);
            offset = offset_to_write;
        } else if (active == 0) {
            fseek(index_file, -1, SEEK_CUR);
            fwrite(&active_to_write, sizeof(uint8_t), 1, index_file);
            fread(&offset, sizeof(uint32_t), 1, index_file);
        }
        fclose(index_file);
    }
    return offset;
}

/*!
 * @brief function add_row_to_table adds a row to a table. Relies on format_row, compute_record_length,
 * find_first_free_record, get_table_definition, open_content_file, open_index_file
 * @param table_name the name of the target table
 * @param record the record to add
 */
void add_row_to_table(char *table_name, table_record_t *record) {
    //Préparation de l'ajout
    table_definition_t *def = malloc(sizeof(table_definition_t));
    def = get_table_definition(table_name, def);
    if (def != NULL) { 
      //Recherche d'une clé primaire
      int i = 0;
      bool key = false;
      while (i < def->fields_count && key == false) {
          if (record->fields[i].field_type == TYPE_PRIMARY_KEY) {
              key = true;
          } else {
              i++;
          }
      }
      uint16_t length = compute_record_length(def);
      uint32_t offset = find_first_free_record(table_name);
      
      char *buffer = malloc(sizeof(char) * length + 1);
      for (int j=0; j<length + 1; j++) {
          buffer[j] = '\0';
      }
      buffer = format_row(table_name, buffer, def, record);
      
      FILE *content_file = open_content_file(table_name, "rb+");
      if (content_file != NULL) {
          fseek(content_file, offset, SEEK_SET);
          fwrite(buffer, length, 1, content_file);
          if (key) {
              update_key(table_name, record->fields[i].field_value.primary_key_value);
          }
      }

      fclose(content_file);
    } else {
      printf("[add_row_to_table] Erreur dans l'ouverture du fichier.");
      exit(EXIT_FAILURE);
    }
}

/*!
 * @brief function format_row prepares the buffer to be written to the content file
 * @param table_name the name of the target table
 * @param buffer the buffer where the row content is prepared
 * @param table_definition the table definition for fields ordering and formatting
 * @param record the record to write
 * @return a pointer to buffer in case of success, NULL else.
 */
char *format_row(char *table_name, char *buffer, table_definition_t *table_definition, table_record_t *record) {
    int i, j, y;
    int position=0;
    bool trouver;
    double tmp;
    FILE *file_temp;
    char *caractere;
    char *name_file_temp={"file_temp.temp"};
    for (i=0; i<table_definition->fields_count; i++) {
        trouver = false;
        j = 0;
        while (j < record->fields_count && trouver == false) {
            if (strcmp(table_definition->definitions[i].column_name, record->fields[j].column_name) == 0) {
                trouver = true;
            } else {
                j++;
            }
        }
        switch (record->fields[j].field_type) {
            case TYPE_PRIMARY_KEY:
                //creation d'un fichier temporaire
                file_temp = fopen(name_file_temp, "wb+");
                //ecriture dans le fichier en mode binaire du nombre
                tmp = record->fields[j].field_value.primary_key_value;
                fwrite(&tmp, sizeof(double), 1, file_temp);
                //deplacement au début du fichier
                fseek(file_temp, 0, SEEK_SET);
                //allocation de la mémoire neccessaire pour strocker le type voulu
                caractere = malloc(sizeof(char) * sizeof(double));
                //lecture du nombre sous forme de caractere
                fread(caractere, sizeof(char), 8, file_temp);
                //concaténation dans le buffer pour utilisation
                for (y=0; y<8; y++) {
                    buffer[position] = caractere[y];
                    position++;
                }
                //liberation de la mémoire et du fichier
                free(caractere);
                fclose(file_temp);
                //suppression du fichier temporaire
                remove(name_file_temp);
                break;
            case TYPE_INTEGER:
                //creation d'un fichier temporaire
                file_temp = fopen(name_file_temp, "wb+");
                //conversion en double
                tmp = record->fields[j].field_value.int_value;
                //ecriture dans le fichier en mode binaire du nombre
                fwrite(&tmp, sizeof(double), 1, file_temp);
                //deplacement au début du fichier
                fseek(file_temp, 0, SEEK_SET);
                //allocation de la mémoire neccessaire pour strocker le type voulu
                caractere = malloc(sizeof(char) * sizeof(double));
                //lecture du nombre sous forme de caractere
                fread(caractere, sizeof(char), 8, file_temp);
                //concaténation dans le buffer pour utilisation
                for (y=0; y<8; y++) {
                    buffer[position] = caractere[y];
                    position++;
                }
                //liberation de la mémoire et du fichier
                free(caractere);
                fclose(file_temp);
                //suppression du fichier temporaire
                remove(name_file_temp);
                break;
            case TYPE_FLOAT:
                //creation d'un fichier temporaire 
                file_temp = fopen(name_file_temp, "wb+");
                //ecriture dans le fichier en mode binaire du nombre
                fwrite(&record->fields[j].field_value.float_value, sizeof(double), 1, file_temp);
                //deplacement au début du fichier
                fseek(file_temp, 0, SEEK_SET);
                //allocation de la mémoire neccessaire pour strocker le type voulu
                caractere = malloc(sizeof(char) * sizeof(double));
                //lecture du nombre sous forme de caractere
                fread(caractere, sizeof(char), 8, file_temp);
                //concaténation dans le buffer pour utilisation
                for (y=0; y<8; y++) {
                    buffer[position] = caractere[y];
                    position++;
                }
                //liberation de la mémoire et du fichier
                free(caractere);
                fclose(file_temp);
                //suppression du fichier temporaire
                remove(name_file_temp);
                break;
            case TYPE_TEXT:
                for (y=strlen(record->fields[j].field_value.text_value); y<150; y++) {
                  record->fields[j].field_value.text_value[y] = '\0';
                }
                for (y=0; y<150; y++) {
                    buffer[position] = record->fields[j].field_value.text_value[y];
                    position++;
                }
                break;
            case TYPE_UNKNOWN:
              printf("[format_row] Erreur dans le type de la valeur pour la transformation en binaire");
              break;
        }
    }
    return buffer;
}

/*!
 * @brief function update_key updates the key value in the key file. Key value is updated if and only if the new value
 * if higher than the stored value. The value sent to the function is the last value inserted into the table, so the
 * function must increment it before comparing to the key file content.
 * @param table_name the name of the table whose key file must be updated
 * @param value the new key value
 */
void update_key(char *table_name, unsigned long long value) {
    FILE *key_file = open_key_file(table_name, "rb+");
    value++;
    if (key_file != NULL) {
        unsigned long long key;
        if (fread(&key, sizeof(unsigned long long), 1, key_file) != 0) {
            if (value > key) {
                fseek(key_file, 0, SEEK_SET);
                fwrite(&value, sizeof(unsigned long long), 1, key_file);
            }
        }
        fclose(key_file);
    }
}

/*!
 * @brief function get_next_key extract the next key value from a table key file.
 * @param table_name the name of the table whose key value we need.
 * @return the next value of the key is it exists, 0 else
 */
unsigned long long get_next_key(char *table_name) {
    FILE *key_file;
    key_file = open_key_file(table_name, "rb");
    if (key_file != NULL) {
        unsigned long long key;
        if (fread(&key, sizeof(unsigned long long), 1, key_file) != 0) {
            return key;
        }
        fclose(key_file);
    }
    return 0;
}

/*!
 * @brief function find_field_in_table_record looks up for a field by its name in a table_record_t structure
 * @param field_name the name of the field to look for
 * @param record a pointer to the table_record_t structure
 * @return a pointer to the matching field_record_t if found, NULL else
 */
field_record_t *find_field_in_table_record(char *field_name, table_record_t *record) {
    return NULL; // Checking the query shall avoid this
}

/*!
 * @brief function is_matching_filter tests a table record against a filter to check if the record matches the filter,
 * i.e. conditions in the filter are verified by the record. The filter contains one or more equalities to check.
 * Tests are all following a single AND or OR logical operator.
 * @param record the record to be checked
 * @param filter the filter to check against (a NULL filter means no filtering must be done -> returns 0)
 * @return true if the record matches the filter, false else
 */
bool is_matching_filter(table_record_t *record, filter_t *filter) {
    bool is_matching = false;
    int matching = 0;
    int j;
    for (int i = 0; i < filter->values.fields_count; i++) {
      j = 0;
      while (filter->values.fields[i].column_name != record->fields[j].column_name && j < record->fields_count) {
        j++;
      }
      if (filter->values.fields[i].column_name == record->fields[j].column_name) { //To check why we left while loop
        switch (filter->values.fields[i].field_type) {
          case TYPE_PRIMARY_KEY:
            if (filter->values.fields[i].field_value.primary_key_value == record->fields[j].field_value.primary_key_value) {
              matching++;
            }
            break;
          case TYPE_INTEGER:
            if (filter->values.fields[i].field_value.int_value == record->fields[j].field_value.int_value) {
              matching++;
            }
            break;
          case TYPE_FLOAT:
            if (filter->values.fields[i].field_value.float_value == record->fields[j].field_value.float_value) {
              matching++;
            }
            break;
          case TYPE_TEXT:
            if (strcmp(filter->values.fields[i].field_value.text_value, record->fields[j].field_value.text_value) == 0) {
              matching++;
            }
            break;
          case TYPE_UNKNOWN:
            printf("[is_matching_filter] Erreur dans le switch type.");
            break;
        }
      }
    }
    if (matching == filter->values.fields_count) {
      is_matching = true;
    }
    return is_matching;
}

/*!
 * @brief function get_filtered_records gets a list (as a linked list) of table records limited to the fields
 * specified to the function and filtered by the specified WHERE clause.
 * @param table_name table from which to fetch data
 * @param required_fields the list of fields to be returned (cannot be NULL)
 * @param filter the WHERE clause filter. NULL if no filter must be applied
 * @param result a pointer to a previously allocated record_list_t. List shall be empty
 * @return a pointer to the first element of the resulting linked list. Shall be freed by the calling function
 */
record_list_t *get_filtered_records(char *table_name, table_record_t *required_fields, filter_t *filter, record_list_t *result) {
  
  return result;
}

/*!
 * @brief function get_table_record reads a table record from its content file
 * @param table_name the table whose record must be read from
 * @param offset the position of the beginning of the record in the content file (found in the index file)
 * @param def the table definition
 * @param result a pointer to a previously allocated table_record_t structure
 * @return the pointer to to result if succeeded, NULL else.
 */
table_record_t *get_table_record(char *table_name, uint32_t offset, table_definition_t *def, table_record_t *result) {
  double temp;
  FILE *content_file = open_content_file(table_name, "rb");
  if (content_file != NULL) {
    fseek(content_file, offset, SEEK_SET);
    for (int i=0; i<def->fields_count; i++) {
        switch (def->definitions[i].column_type) {
            case TYPE_PRIMARY_KEY:
                fread(&result->fields[i].field_value.primary_key_value, sizeof(unsigned long long), 1, content_file);
                result->fields[i].field_type = TYPE_PRIMARY_KEY;
                break; 
            case TYPE_INTEGER:
                fread(&temp, sizeof(double), 1, content_file);
                result->fields[i].field_value.int_value = temp;
                result->fields[i].field_type = TYPE_INTEGER;
                break;
            case TYPE_FLOAT:
                fread(&result->fields[i].field_value.float_value, sizeof(double), 1, content_file);
                result->fields[i].field_type = TYPE_FLOAT;
                break;
            case TYPE_TEXT:
                fread(&result->fields[i].field_value.text_value, sizeof(char), 150, content_file);
                result->fields[i].field_type = TYPE_TEXT;
                break;
            default: 
                printf("[get_table_record] Erreur dans le switch");
                break;
        }
        strcpy(result->fields[i].column_name, def->definitions[i].column_name);
    }
    result->fields_count = def->fields_count;
  }
  return result;
}
