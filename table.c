//
// Created by flassabe on 19/11/2021.
//

#include "table.h"

#include <sys/stat.h>
#include <unistd.h>

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
        file = malloc(sizeof(char) * (strlen(table_name) + strlen(".def")));
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
        file = malloc(sizeof(char) * (strlen(table_name) + strlen(".idx")));
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
        file = malloc(sizeof(char) * (strlen(table_name) + strlen(".data")));
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
    if (table_exists(table_name)) {
        chdir(table_name);
        file = malloc(sizeof(char) * (strlen(table_name) + strlen(".key")));
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
    if (directory_exists(table_name)) {
        return 1;
    } else {
        return 2;
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
    int i, N;
    int val_bin = 0;
    int j = 0;
    //int lenth_table_name = strlen(table_definition->table_name);

    FILE *def_file = NULL;
    FILE *key_file = NULL;
    FILE *index_file = NULL;
    FILE *content_file = NULL;

    //on crée le dossier de la table et on se déplace dedant
    _mkdir(table_definition->table_name);
    chdir(table_definition->table_name);

    //on crée le fichier de definition
    def_file = open_definition_file(table_definition->table_name, "w+"); 

    // on crée les fichiers d'index et de contenu
    index_file = open_index_file(table_definition->table_name, "w+");
    content_file = open_content_file(table_definition->table_name, "w+");

    // on les referme pour les sauvegarder dans le directory nommé : table_name
    fclose(index_file);
    fclose(content_file);

    while (j < table_definition->table_definition.fields_count) {
        if (table_definition->table_definition.definitions[j].column_type == TYPE_PRIMARY_KEY) {
            key_file = open_key_file(table_definition->table_name, "w+");
            fwrite("1\n", 1, sizeof(int), key_file);
        }
        fprintf(def_file, "%d %s\n", table_definition->table_definition.definitions[j].column_type, table_definition->table_definition.definitions[j].column_name);
        j++;
    }

    /*
    for( i=0; (i<MAX_FIELDS_COUNT); i++){
        switch (table_definition->table_definition.definitions[i].column_type){
            case TYPE_UNKNOWN:
                N=0;
                break;
            case TYPE_PRIMARY_KEY:
                N=1;
                key_file=open_definition_file(table_definition->table_name, "w");
                if(key_file==NULL){
                    printf("erreur, le fichier .key n'a pas pu être ouvert\n");
                    break;
                }else{
                  val_bin=1;   
                }
                break;
            case TYPE_INTEGER:
                N=2;
                break;
            case TYPE_FLOAT:
                N=3;
                break;
            case TYPE_TEXT:
                N=4;
                break;
            default:
                printf("erreur!\n");
        }// end switch
        fprintf(def_file,"%d %s\n",N,table_definition->table_definition.definitions[i].column_name);      
    }// end for
    */
    chdir("..");
    fclose(key_file);
    fclose(def_file);
}

/*!
 * @brief function drop_table removes all files and directory related to a table
 * @param table_name the name of the dropped table.
 */
void drop_table(char *table_name){
    if (directory_exists(table_name) == true) {
      printf("la table %s va être supprimée\n", table_name);
      recursive_rmdir(table_name);
    } else {
        printf("la table %s est déjà supprimée/n'existe pas/plus\n", table_name);
    }
}

void drop_database(char *db_name)
{
    if (directory_exists(db_name)==true){
      printf("la base de données %s va être supprimée avec toutes les tables qu'elle contient\n", db_name);
      recursive_rmdir(db_name);
    }else{
        printf("la base de données  %s est déjà supprimée/n'existe pas/plus\n", db_name);
    }
}

/*!
 * @brief function get_table_definition reads and returns a table definition
 * @param table_name the name of the target table
 * @param result a pointer to a previously allocated table_definition_t structure where the result will be stored.
 * @return the pointer to result, NULL if the function failed
 */
table_definition_t *get_table_definition(char *table_name, table_definition_t *result) {
    if (table_exists(table_name)) {
        chdir(table_name);
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
        chdir("..");
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
    uint32_t offset = 0;
    return offset;
}

/*!
 * @brief function add_row_to_table adds a row to a table. Relies on format_row, compute_record_length,
 * find_first_free_record, get_table_definition, open_content_file, open_index_file
 * @param table_name the name of the target table
 * @param record the record to add
 */
void add_row_to_table(char *table_name, table_record_t *record) {
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
}

/*!
 * @brief function get_next_key extract the next key value from a table key file.
 * @param table_name the name of the table whose key value we need.
 * @return the next value of the key is it exists, 0 else
 */
unsigned long long get_next_key(char *table_name) {
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
    return false;
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
    return result;
}
