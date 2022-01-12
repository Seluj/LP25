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
  if (sql != NULL) {
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
  if (sql != NULL) {
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
  if (sql != NULL) {
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
  if (sql != NULL) {
    char *temp;
    temp = get_sep_space(sql);
    if (temp != NULL) {
      sql = temp;
    }
    temp = field_name;
    while (*temp != '\0') {
      *temp = '\0';
      temp++;
    }
    if (*sql != ';' && *sql != ')' && *sql != ',' && *sql != '=' && *sql != '(' ) {
    // 39 corresponds to '
      if (*sql == 39) {
          sql++;
          while (*sql != 39 && *sql != '\0') {
              *field_name = *sql;
              sql++;
              field_name++;
          }
          if (*sql == '\0') {
            return NULL;
          } 
          sql++;
      } else {
          while (*sql != ' ' && *sql != ';' && *sql != ')' && *sql != ',' && *sql != '=' && *sql != '(') {
              *field_name = *sql;
              sql++;
              field_name++;
          }
      }
    } else {
      sql = NULL;
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
    char name_or_value[150];
    bool end = false;
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
            for (int j=0; j<150; j++) {
                name_or_value[j]='\0';
            }
            //We gather the field or value
            sql = get_field_name(sql, name_or_value);
            strcpy(result->fields[result->fields_count].field_value.text_value, name_or_value);
            //We go on to the next field or value [if there is one of course]
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
            result->fields_count++;
        }
    } else if (*sql == '*') {
      result->fields[result->fields_count].field_value.text_value[0] = '*';
      sql = get_sep_space_and_char(sql, '*');
      result->fields_count = 1;
      return sql;
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
}//DONE 

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
  for (int j = 0; j < TEXT_LENGTH; j++) {
    name[j] = '\0';
  }
  sql = get_field_name(sql, equality_name);
  if (sql != NULL) {
    strcpy(equality->column_name, name);
    sql = get_sep_space_and_char(sql, '=');
    if (sql != NULL) {
      for (int j = 0; j < TEXT_LENGTH; j++) {
        name[j] = '\0';
      }
      sql = get_field_name(sql, name);
      if (sql != NULL) {
        strcpy(equality->field_value.text_value, equality_name);
        equality->field_type = TYPE_UNKNOWN;
      } else {
        printf("\n[parse_equality] Erreur dans votre égalité. Il manque la valeur\n");
      }
    } else {
      printf("\n[parse_equality] Erreur dans votre égalité. Il manque le signe égalité\n");
    }
  } else {
    printf("\n[parse_equality] Erreur dans votre égalité. Il manque le field\n");
  }
  return sql;
}//DONE

/*!
*@brief Function parses a SET query composed of at least one equality
*@param Pointer on specific position in a string
*@param Pointer on structure where we will store the different equalities
*@return Pointer on the next character following the equality  
*/
char *parse_set_clause(char *sql, table_record_t *result) {
  bool end = false;
  result->fields_count = 0;
  while (end == false && sql != NULL) {
    sql = parse_equality(sql, &result->fields[result->fields_count]);
    if (sql != NULL) {
      result->fields_count++;
      if (get_sep_space_and_char(sql, ',') == NULL){
        end = true;
      } else {
        sql = get_sep_space_and_char(sql, ',');
      }
    } else {
      printf("\n[parse_set_clause] Erreur dans une des égalités\n");
    }     
  }
  return sql;
}//DONE

/*!
*@brief Function parses a WHERE query composed of at least one equality
*@param Pointer on specific position in a string
*@param Pointer on structure where we will store the different equalitues
*@return Pointer on the next character following the equality  
*/
char *parse_where_clause(char *sql, filter_t *filter) {
  bool end = false;
  filter->values.fields_count = 0;
  if (get_keyword(sql, "WHERE") != NULL) {
    sql = get_keyword(sql, "WHERE");
    sql = parse_equality(sql, &filter->values.fields[filter->values.fields_count]);
    if (sql != NULL) {
      filter->values.fields_count++;
      if (get_keyword(sql, "AND") != NULL){
        sql = get_field_name(sql, "AND");
        filter->logic_operator = OP_AND;
      } else if (get_keyword(sql, "OR") != NULL) {
        sql = get_field_name(sql, "OR");
        filter->logic_operator = OP_OR;
      } else {
        filter->logic_operator = OP_ERROR;
        end = true;
      }
      while (sql != NULL && end == false) {
        sql = parse_equality(sql, &filter->values.fields[filter->values.fields_count]);
        if (sql != NULL) {
          filter->values.fields_count++;
          if (filter->logic_operator == OP_AND) {
            if (get_keyword(sql, "AND") != NULL) {
              sql = get_keyword(sql, "AND");
            } else {
              end = true;
            }
          } else if (filter->logic_operator == OP_OR) {
            if (get_keyword(sql, "OR") != NULL) {
              sql = get_keyword(sql, "OR");
            } else {
              end = true;
            }
          } else {
            end = true;
          }
        } else {
          printf("\n[parse_where_clause] Erreur avec l'égalité.\n");
        }
      }
    } else {
      printf("\n[parse_where_clause] Erreur avec l'égalité.\n");
    }
  }
  return sql;
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
  printf("\nCommande entrée : \n\t%s\n\n", sql);
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
    if (sql != NULL) {
      sql = get_keyword(sql, "FROM");
      if (sql != NULL) {
        sql = get_field_name(sql, pointer_field);
        if (sql != NULL) {
          strcpy(result->query_content.select_query.table_name, field_name);
          if (sql != NULL) {
            if (parse_where_clause(sql, &result->query_content.select_query.where_clause) != NULL) {
              sql = parse_where_clause(sql, &result->query_content.select_query.where_clause);
            } else {
              result->query_content.select_query.where_clause.values.fields_count = 0;
            }
            if (has_reached_sql_end(sql) != true) {
              printf("\n[parse_select] La fin de la commande n'est pas correct\n");
                result = NULL;
            }  
          } else {
            result = NULL;
          }
        } else {
          printf("\n[parse_select] Veuillez entrer un nom de tableau avec la bonne syntaxe.\n");
          result = NULL;
        }
      } else {
        printf("\n[parse_select] Il manque 'FROM'.\n");
        result = NULL;
      }
    } else {
      printf("\n[parse_select] Veuillez entrer une liste de champs ou un asterix avec la bonne syntaxe.\n");
      result = NULL;
    }

    return result;
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
    printf("\n[parse_create] Le nom de tableau est trop long\n");
    result = NULL;
  } else {
    strcpy(result->query_content.create_query.table_name, table_name);
    sql = parse_create_fields_list(sql, &result->query_content.create_query.table_definition);
    if (sql == NULL) {
      printf("\n[parse_create] Veuillez vérifier la syntaxe des champs et types entrés\n");
      result = NULL;
    } else {
      if (has_reached_sql_end(sql) != true) {
        printf("\n[parse_create] Veuillez vérifier la syntaxe de la fin de votre requète\n");
        result = NULL;
      }
    }
  }
  return result;
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
      strcpy(result->query_content.insert_query.table_name, field_name);
      sql = get_field_name(sql, pointer_field);
      sql = parse_fields_or_values_list(sql, &result->query_content.insert_query.fields_names);
      if (sql != NULL) {
        sql = get_keyword(sql, "VALUES");
        if (sql != NULL) {
          sql = parse_fields_or_values_list(sql, &result->query_content.insert_query.fields_values);
          if (sql != NULL) {
            if (has_reached_sql_end(sql) != true) {
              printf("\n[parse_insert] La fin de votre commande n'est pas correct.\n");
              result = NULL;
            }
          } else {
            printf("\n[parse_insert] Les valeurs entrées ne respectent pas la bonne syntaxe. ex: (marley, bob)\n");
            result = NULL;
          }
        } else {
          printf("\n[parse_insert] Il manque 'VALUES'.\n");
          result = NULL;
        }
      } else {
        printf("\n[parse_insert] Les champs entrés ne respectent pas la bonne syntaxe. ex: (nom, prénom)\n");
        result = NULL;
      }
    } else {
      printf("\n[parse_insert] Veuillez entrer un nom de table. \n");
      result = NULL;
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
  char field_name[TEXT_LENGTH];
  char *pointer_field = &field_name[0];
  sql = get_field_name(sql, pointer_field);
  if (sql != NULL) {
    strcpy(result->query_content.update_query.table_name, field_name);
    sql = get_keyword(sql, "SET");
    if (sql != NULL) {
      sql = parse_set_clause(sql, &result->query_content.update_query.set_clause);
      if (sql != NULL) {
        if (parse_where_clause(sql, &result->query_content.update_query.where_clause) != NULL) {
          sql = parse_where_clause(sql, &result->query_content.update_query.where_clause);
        } else {
          result->query_content.select_query.where_clause.values.fields_count = 0;
        }
        if (has_reached_sql_end(sql) != true) {
          printf("\n[parse_update] Veuillez vérifier la syntaxe de fin.\n");
          result = NULL;
        }
      } else {
        printf("\n[parse_update] Veuillez vérifier vos conditions et la syntaxe.\n");
        result = NULL;
      }
    } else {
      printf("\n[parse_update] Il manque 'SET' ou votre requète est mal formée.\n");
      result = NULL;
    }
  } else {
    printf("\n[parse_update] Veuillez rentrer un nom de table en respectant la syntaxe.\n");
    result = NULL;
  }
  return result;
}

/*!
*@brief Function calls the proper functions for a DELETE query
*@param Pointer on specific position in a string
*@param Pointer on structure where we will store the different query parameters
*@return Pointer on the structure where the different query parameters are stored
*/
query_result_t *parse_delete(char *sql, query_result_t *result) {
  char field_name[TEXT_LENGTH];
  char *pointer_field = &field_name[0];
  sql = get_field_name(sql, pointer_field);
  if (sql != NULL) {
    strcpy(field_name, result->query_content.delete_query.table_name);
    if (parse_where_clause(sql, &result->query_content.delete_query.where_clause) != NULL ) {
      sql = parse_where_clause(sql, &result->query_content.delete_query.where_clause);
    } else {
      result->query_content.select_query.where_clause.values.fields_count = 0;
    }
    if ( has_reached_sql_end(sql) != true) {
      printf("\n[parse_update] Veuillez vérifier la syntaxe de fin.\n");
      result = NULL;
    } 
  } else {
    printf("\n[parse_update] Veuillez rentrer un nom de table en respectant la syntaxe.\n");
    result = NULL;
  }
  return result;
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
    printf("\n[parse_drop_db] Veuillez rentrer un nom de tableau en respectant la syntaxe.\n");
    result = NULL;
  } else {
    strcpy(result->query_content.database_name, database_name);
    if (has_reached_sql_end(sql) != true) {
      printf("\n[parse_drop_db] Veuillez vérifier la fin de votre commande.\n");
      result = NULL;
    }
  }
  return result;
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
      printf("\n[parse_drop_table] Veuillez rentrer un nom de base de donnée en respectant la syntaxe.\n");
      result = NULL;
    } else {
      strcpy(result->query_content.table_name, table_name);
      if (has_reached_sql_end(sql) != true) {
        printf("\n[parse_drop_table] Veuillez vérifier la fin de votre commande.\n");
        result = NULL;
      }
    }
    return result;
} //DONE