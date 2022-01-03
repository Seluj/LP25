//
// Created by flassabe on 16/11/2021.
//

#include "sql.h"

#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>

/*!
*@brief Function checks the presence of a sequence of spaces [1 to an undetermined number], followed by a non-space character
*@param Pointer on a specific position in a string
*@return Pointer on the first non-space character
*/
char *get_sep_space(char *sql) {
    int numb_spaces = 0;
    while (*sql == ' ') {
        numb_spaces++;
        sql++;
    }
    if (numb_spaces != 0 ) {
        return sql;
    } else {
        return NULL;
    }
}//DONE

/*!
*@brief Function checks the presence of a sequence or not of zeros, the character c, and another sequence or not of zeros
*@param Pointer on a specific position in a string
*@param A specific character we are looking for in the pointed string
*@return Pointer on the next non-space character following the character c surrounded or not by sequences of spaces.
*/
char *get_sep_space_and_char(char *sql, char c) {
    while (*sql == ' ') {
        sql++;
    }
    //We check that the next non-space character is the one we are looking for
    if (*sql == c) {
        sql++;
        while (*sql == ' ') {
            sql++;
        }
        return sql;
    } else {
        return NULL;
    }
}//DONE

/*!
*@brief Function checks that the first word in the pointed string corresponds to the keyword in the parameters
*@param Pointer on a specific position in a string
*@param A specific keyword
*@return Pointer on the character following the specific keyword or null if the keyword isn't found
*/
char *get_keyword(char *sql, char *keyword) {
    char word[50];
    char first_word[50];
    int i = 0;
    for (int j=0; j<50; j++) {
      word[j] = '\0';
      first_word[j] = '\0';
    }
    char *temp = sql;
    temp = get_sep_space(sql);
    if (temp != NULL) {
      sql = temp;
    }
    //We get the first word from the sql string and put it in capital letters
    while (i < 50 && *sql != ' ' && *sql != ';') {
        first_word[i] = toupper(*sql);
        i++;
        sql++;
    }
    //We also put the keyword in capital letters for safe measures
    i = 0;
    while (*keyword != '\0') {
        word[i] = toupper(*keyword);
        i++;
        keyword++;
    }
    if (strcmp(word, first_word) == 0) {
        temp = get_sep_space(sql);
        if (temp != NULL){
          sql = temp;
        }
        return sql;
    } else {
        return NULL;
    }
}//DONE

/*!
*@brief Function extracts a field name and changes pointer position on to the character following the extracted field name
*@param Pointer on a specific position in a string
*@param Buffer containing the extracted value
*@return Pointer on the character following the value we've gathered
*/
char *get_field_name(char *sql, char *field_name) {
    char *temp;
    temp = get_sep_space(sql);
    if (temp != NULL) {
      sql = temp;
    }
    temp = field_name;
    while (*field_name != '\0') {
      *field_name = '\0';
      field_name++;
    }
    // 39 corresponds to '
    if (*sql == 39) {
        sql++;
        while (*sql != 39 && *sql != '\0') {
            *field_name = *sql;
            sql++;
            field_name++;
        }
        if (sql == '\0') {
          return NULL;
        } 
        sql++;
    } else {
        while (*sql != ' ' && *sql != ';' && *sql != ')' && *sql != ',' && *sql != '=') {
            *field_name = *sql;
            sql++;
            field_name++;
        }
    }
    return sql;
}//DONE

/*!
*@brief Function tests if we've reached the end of the string 
*@param Pointer on a specific position in a string
*@return Returns false if we haven't reached the end and true if we have.
*/
bool has_reached_sql_end(char *sql) {
  bool end = false;  
  while (*sql == ' ') {
    sql++;
  }
  if (*sql == ';') {
    end = true;
  }
  return end;
}//DONE

/*!
*@brief Function extracts a list of fields or values (this type of list is seperated by commas)
*@param Pointer on a specific position in a string
*@param Pointer on a structure where we sill stock the list of fields or values we find
*@return Pointer on the next character following the list of fields or values we've found
*/
char *parse_fields_or_values_list(char *sql, table_record_t *result) {
    char name_or_value[TEXT_LENGTH];
    char *field_name_or_value = &name_or_value[0];
    int j = 0;
    char *temp;
    bool end = false;
    bool parentheses = false; //Indicates if the list is in parentheses
    result->fields_count = 0 ;
    temp = get_sep_space(sql);
    if (temp != NULL) {
        sql = temp;
    }
    if (*sql == '(') {
        sql++;
        parentheses = true;
    }
    while (end != true && j < MAX_FIELDS_COUNT && sql != NULL) {
      for (int i = 0; i < TEXT_LENGTH; i++) {
        name_or_value[i] = '\0';
      }
        sql = get_field_name(sql, field_name_or_value);
        if (sql != NULL) {
          strcpy(result->fields[j].field_value.text_value, name_or_value);
          if (get_sep_space_and_char(sql, ',') == NULL) {
            end = true;
            if (get_sep_space_and_char(sql,')') == NULL && parentheses == true) {
              sql =  NULL;
            } else if (get_sep_space_and_char(sql,')') != NULL && parentheses == true) {
              sql = get_sep_space_and_char(sql,')');
            }
          } else {
            sql = get_sep_space_and_char(sql, ',');
          }
          result->fields_count = result->fields_count + 1;
        }
    }
    return sql;
}//DONE but issue if no closing parentheses

/*!
*@brief Function extracts a list of fields (this type of list is seperated by commas, however the field and field type are seperated by a space)
*@param Pointer on a specific position in a string
*@param Pointer on a structure where we will stock the list of fields and field types
*@return Pointer on the next character following the list of fields
*/
char *parse_create_fields_list(char *sql, table_definition_t *result) {
    char name_or_type[150];
    for (int j=0; j<150; j++) {
        name_or_type[j]='\0';
    }
    bool end = false;
    char *field_name_or_type = &name_or_type[0];
    field_type_t type; 
    int i;
    char *temp;
    result->fields_count = 0;
    temp = get_sep_space(sql);
    if (temp != NULL) {
      sql = temp;
    }
    if (*sql == '(') {
        sql++;
        while (end == false && *sql != ')') {
            //First we gather the field name
            sql = get_field_name(sql, name_or_type);
            strcpy(result->definitions[result->fields_count].column_name, name_or_type);
            for (int j=0; j<150; j++) {
                name_or_type[j]='\0';
            }
            //Then we gather the field type
            sql = get_field_name(sql, field_name_or_type);
            i = 0;
            while (name_or_type[i] != '\0') {
                name_or_type[i] = toupper(name_or_type[i]);
                i++;
            }
            if (strcmp(name_or_type, "INT") == 0) {
                type = TYPE_INTEGER;
            } else if (strcmp(name_or_type, "PRIMARY KEY") == 0) {
                type = TYPE_PRIMARY_KEY;
            } else if (strcmp(name_or_type, "FLOAT") == 0) {
                type = TYPE_FLOAT;
            } else if (strcmp(name_or_type, "TEXT") == 0) {
                type = TYPE_TEXT;
            } else {
                type = TYPE_UNKNOWN;
            }
            for (int j=0; j<150; j++) {
                name_or_type[j]='\0';
            }
            //We go on to the next field name and type [if there is one of course]
            temp = get_sep_space_and_char(sql, ',');
            if (temp == NULL) {
              end = true;
              temp = get_sep_space(sql);
              if (temp != NULL) {
                sql = temp;
              }
            } else {
              sql = get_sep_space_and_char(sql, ',');
            }
            result->definitions[result->fields_count].column_type = type;
            result->fields_count++;
        }
    } else {
        printf("Les champs donnés ne sont pas correctement notés\n");
        return NULL;
    }
    if (*sql == ')') {
        sql = get_sep_space_and_char(sql, ')');
        return sql;
    } else {
        printf("\nErreur de syntaxe\n");
        return NULL;
    }
} //DONE

/*!
*@brief Function exctracts equality
*@param Pointer on specific position in a string
*@param Pointer on structure where we will store the equality
*@return Pointer on the next character following the equality  
*/
char *parse_equality(char *sql, field_record_t *equality) {
    char name[TEXT_LENGTH];
    char *equality_name = &name[0];
    sql = get_field_name(sql, equality_name);
    strcpy(equality->column_name, equality_name);
    sql = get_sep_space_and_char(sql, '=');
    sql = get_field_name(sql, equality_name);
    strcpy(equality->field_value.text_value, equality_name);
    equality->field_type = TYPE_UNKNOWN;
    return sql;
}

/*!
*@brief Function parses a SET query composed of at least one equality
*@param Pointer on specific position in a string
*@param Pointer on structure where we will store the different equalities
*@return Pointer on the next character following the equality  
*/
char *parse_set_clause(char *sql, table_record_t *result) {
    char field_name[TEXT_LENGTH];
    char value[TEXT_LENGTH];
    char field_and_value[TEXT_LENGTH+TEXT_LENGTH];
    char *pointer_field_and_value = &field_and_value[0];
    char *temp;
    int i;
    bool end = false;
    sql = get_field_name(sql, pointer_field_and_value);
    while (end == false && sql != NULL) {
      //We reset the string of caracters
      for (int j = 0; j < TEXT_LENGTH; j++) {
        field_name[j] = '\0';
        value[j] = '\0';
      }
      for (int j = 0; j < TEXT_LENGTH; j++) {
        field_and_value[j] = '\0';
      }
      //We gather the field_name
      i = 0;
      while (field_and_value[i] != '=' && field_and_value[i] != '\0'){ 
          field_name[i] = field_and_value[i];
          i++;
      }
      if (*pointer_field_and_value == '=' && field_name[0] != '\0') {

        i = 0;
        while (*pointer_field_and_value != '\0') {
          value[i] = *pointer_field_and_value;
          i++;
          pointer_field_and_value++;
        }
        if (value[0] != '\0') {
          temp = get_sep_space_and_char(sql, ',');
          if (temp == NULL){
            end = true;
          } else {
            sql = temp;
            sql = get_field_name(sql, pointer_field_and_value);
          }
        } else {
          sql = NULL;
        }
      } else {
            sql = NULL;
      }
    }
    return sql;
}

/*!
*@brief Function parses a WHERE query composed of at least one equality
*@param Pointer on specific position in a string
*@param Pointer on structure where we will store the different equalitues
*@return Pointer on the next character following the equality  
*/
char *parse_where_clause(char *sql, filter_t *filter) {
    char *temp;
    int i = 0;
    char operator_one[5];
    char operator[5];
    char *pointer_operator = &operator[0];
    bool end = false;
    sql = get_keyword(sql, "WHERE");
    if (sql != NULL) {
      sql = parse_equality(sql, &filter->values.fields[i]);
      if (get_keyword(sql, "AND") != NULL){
        strcpy(operator_one, "AND");
        sql = get_keyword(sql, "AND");
        filter->logic_operator = OP_AND;
      } else if (get_keyword(sql, "OR") != NULL) {
        strcpy(operator_one, "OR");
        sql = get_keyword(sql, "OR");
        filter->logic_operator = OP_OR;
      } else {
        end = true;
      }
      temp = sql;
      while (strcmp(operator, operator_one) == 0 && temp != NULL && sql != NULL && end == false) {
        sql = temp;
        sql = parse_equality(sql, &filter->values.fields[i]);
        temp = get_field_name(sql, pointer_operator);
        i++;
      }
      if (sql == NULL) {
        return NULL;
        filter->logic_operator = OP_ERROR;
      } else {
        return sql;
      }
    } else {
        return NULL;
    }
}

/*!
*@brief Function calls the proper parse function according to the query entered by the user
*@param Pointer on specific position in a string
*@param Pointer on structure where we will store the different query parameters
*@return Pointer on the structure where the different query parameters are stored
*/
query_result_t *parse(char *sql, query_result_t *result) {
  char *temp = sql;
  bool comma;
  while (*temp != ';' && *temp != '\0') {
    temp++;
  }
  if (*temp == ';') {
    comma = true;
  } else {
    comma = false;
  }
  printf("Commande entrée : \n\t%s\n\n", sql);
  if (comma == true) {
    if (get_keyword(sql, "SELECT") != NULL) {
      sql = get_keyword(sql, "SELECT");
      result->query_type = 3;
      return parse_select(sql, result); 
    } else if (get_keyword(sql, "INSERT") != NULL) {
      sql = get_keyword(sql, "INSERT");
      if (get_keyword(sql, "INTO") != NULL) {
        sql = get_keyword(sql, "INTO");
        result->query_type = QUERY_INSERT;
        return parse_insert(sql, result);
      } else {
        return NULL;
      }
    } else if (get_keyword(sql, "CREATE") != NULL) {
      sql = get_keyword(sql, "CREATE");
      if (get_keyword(sql, "TABLE") != NULL) {
        result->query_type = QUERY_CREATE_TABLE;
        sql = get_keyword(sql, "TABLE");
        result = parse_create(sql, result);
        return result;
      } else {
          printf("Create found but table is missing\n");
        return NULL;
      }
    } else if (get_keyword(sql, "UPDATE") != NULL) {
      result->query_type = QUERY_UPDATE;
      sql = get_keyword(sql, "UPDATE");
      return parse_update(sql, result);
    } else if (get_keyword(sql, "DELETE") != NULL) {
      sql = get_keyword(sql, "DELETE");
      if (get_keyword(sql, "FROM") != NULL) {
        result->query_type = QUERY_DELETE;
        sql = get_keyword(sql, "FROM");
        return parse_delete(sql, result);
      } else {
        return NULL;
      }
    } else if (get_keyword(sql, "DROP") != NULL) {
      sql = get_keyword(sql, "DROP");
      if (get_keyword(sql, "TABLE") != NULL) {
        result->query_type = QUERY_DROP_TABLE;
        sql = get_keyword(sql, "TABLE");
        return parse_drop_table(sql, result);
      } else if (get_keyword(sql, "DATABASE") != NULL || get_keyword(sql, "DB") != NULL) {
        result->query_type = QUERY_DROP_DB;
        sql = get_keyword(sql, "DATABASE");
        return parse_drop_db(sql, result);
      } else {
        return NULL;
      }
    } else {
      return NULL;
    }
  } else {
      printf("Il manque le point virgule\n");
    return NULL;
  }
}//DONE

/*!
*@brief Function calls the proper functions for a SELECT query
*@param Pointer on specific position in a string
*@param Pointer on structure where we will store the different query parameters
*@return Pointer on the structure where the different query parameters are stored
*/
query_result_t *parse_select(char *sql, query_result_t *result) {
    char field_name[TEXT_LENGTH];
    char *pointer_field = &field_name[0];
    sql = parse_fields_or_values_list(sql, &result->query_content.select_query.set_clause);
    sql = get_keyword(sql, "FROM");
    sql = get_field_name(sql, pointer_field);
    strcpy(result->query_content.select_query.table_name, field_name);
    if (sql != NULL) {
        if (parse_where_clause(sql, &result->query_content.select_query.where_clause) != NULL) {
            return result;
        } else {
            if (has_reached_sql_end(sql) == true) {
                return result;
            } else {
                return NULL;
            }
        }
    } else {
        return NULL;
    }
}

/*!
*@brief Function calls the proper functions for a CREATE query
*@param Pointer on specific position in a string
*@param Pointer on structure where we will store the different query parameters
*@return Pointer on the structure where the different query parameters are stored
*/
query_result_t *parse_create(char *sql, query_result_t *result) {
    char table_name[TEXT_LENGTH];
    char *pointer_name = &table_name[0];
    int i = 0;
    if (get_sep_space(sql) != NULL) {
        sql = get_sep_space(sql);
    }
    sql = get_field_name(sql, pointer_name);
    if (strlen(table_name) == TEXT_LENGTH-1) {
        return NULL;
    } else {
        strcpy(result->query_content.create_query.table_name, table_name);
        sql = parse_create_fields_list(sql, &result->query_content.create_query.table_definition);
        if (sql == NULL) {
            return NULL;
        } else {
            if (has_reached_sql_end(sql) == true) {
              return result;
            } else {
                return NULL;
            }
        }
    }
} //DONE

/*!
*@brief Function calls the proper functions for a INSERT query
*@param Pointer on specific position in a string
*@param Pointer on structure where we will store the different query parameters
*@return Pointer on the structure where the different query parameters are stored
*/
query_result_t *parse_insert(char *sql, query_result_t *result) {
    char field_name[TEXT_LENGTH];
    char *pointer_field = &field_name[0];
    if (get_field_name(sql, pointer_field) != NULL){
      sql = get_field_name(sql, pointer_field);
      strcpy(result->query_content.insert_query.table_name,field_name);
      sql = parse_fields_or_values_list(sql, &result->query_content.insert_query.fields_names);
      sql = get_keyword(sql, "VALUES");
      if (sql != NULL) {
        sql = parse_fields_or_values_list(sql, &result->query_content.insert_query.fields_values);
        if (has_reached_sql_end(sql) != true) {
          sql = NULL;
        }
      }
    }
  return result;
}//DONE

/*!
*@brief Function calls the proper functions for an UPDATE query
*@param Pointer on specific position in a string
*@param Pointer on structure where we will store the different query parameters
*@return Pointer on the structure where the different query parameters are stored
*/
query_result_t *parse_update(char *sql, query_result_t *result) {
    char *temp;
    char field_name[TEXT_LENGTH];
    char *pointer_field = &field_name[0];
    sql = get_field_name(sql, pointer_field);
    strcpy(result->query_content.update_query.table_name, field_name);
    sql = get_keyword(sql, "SET");
    sql = parse_set_clause(sql, &result->query_content.update_query.set_clause);
    if (parse_where_clause(sql, &result->query_content.update_query.where_clause) != NULL) {
      sql = parse_where_clause(sql, &result->query_content.update_query.where_clause);
    }
    if (has_reached_sql_end(sql) == true) {
      return result;
    } else {
        return NULL;
    }
}

/*!
*@brief Function calls the proper functions for a DELETE query
*@param Pointer on specific position in a string
*@param Pointer on structure where we will store the different query parameters
*@return Pointer on the structure where the different query parameters are stored
*/
query_result_t *parse_delete(char *sql, query_result_t *result) {
    char *temp;
    char field_name[TEXT_LENGTH];
    char *pointer_field = &field_name[0];
    sql = get_field_name(sql, pointer_field);
    if (sql != NULL) {
        strcpy(field_name, result->query_content.delete_query.table_name);
        temp = parse_where_clause(sql, &result->query_content.delete_query.where_clause);
        if (temp != NULL ) {
            sql = temp;
        }
        if ( has_reached_sql_end(sql) == true) {
            return result;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

/*!
*@brief Function calls the proper functions for a DROP DATABASE query
*@param Pointer on specific position in a string
*@param Pointer on structure where we will store the different query parameters
*@return Pointer on the structure where the different query parameters are stored
*/
query_result_t *parse_drop_db(char *sql, query_result_t *result) {
    char database_name[TEXT_LENGTH];
    char *database = &database_name[0];
    sql = get_field_name(sql, database);
    if (sql == NULL) {
        return NULL;
    } else {
        if (has_reached_sql_end(sql) == true) {
            strcpy(result->query_content.database_name, database_name);
            return result;
        } else {
            return NULL;
        }
    }
} //DONE

/*!
*@brief Function calls the proper functions for a DROP TABLE query
*@param Pointer on specific position in a string
*@param Pointer on structure where we will store the different query parameters
*@return Pointer on the structure where the different query parameters are stored
*/
query_result_t *parse_drop_table(char *sql, query_result_t *result) {
    char table_name[TEXT_LENGTH];
    char *table = &table_name[0];
    sql = get_field_name(sql, table);
    if (sql == NULL) {
        return NULL;
    } else {
        if (has_reached_sql_end(sql) == true) {
            strcpy(result->query_content.table_name, table_name);
            return result;
        } else {
            return NULL;
        }
    }
} //DONE