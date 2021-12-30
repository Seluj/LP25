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
  char *first_nonspace_char = NULL; //Points on the first non-space character
  int numb_spaces = 0;
  while (*sql != '\0' && *sql == ' ') {
    numb_spaces++;
    sql++;
  }
  if (numb_spaces != 0 ) { 
    first_nonspace_char = sql;
  }
  return first_nonspace_char; //returns NULL if there wasn't one or a sequence of spaces before the first non-space character
}

/*!
*@brief Function checks the presence of a sequence or not of zeros, the character c, and another sequence or not of zeros
*@param Pointer on a specific position in a string
*@param A specific character we are looking for in the pointed string
*@return Pointer on the next non-space character following the character c surrounded or not by sequences of spaces.
*/
char *get_sep_space_and_char(char *sql, char c) {
  char *next_char = NULL;
  int numb_spaces = 0;
  while (*sql == ' ') {
    numb_spaces++;
    sql++;
  }
  //We check that the next non-space character is the one we are looking for
  if (*sql == c){
    sql++;
    while (*sql == ' ') {
      numb_spaces++;
      sql++;
    }
    next_char = sql;
  }
    return next_char; //the pointer returned will be null if we didn't find the specific character surrounded or not by a sequence of zeros
}

/*!
*@brief Function checks that the first word in the pointed string corresponds to the keyword in the parameters
*@param Pointer on a specific position in a string
*@param A specific keyword
*@return Pointer on the character following the specific keyword or null if the keyword isn't found
*/
char *get_keyword(char *sql, char *keyword) {
    char *after_keyword = NULL;
    char word[15];
    char first_word[15]; 
    //Size explanation: We know the keyword will either be select, create, ... therefore we know the keyword if found should be able to fit in less than 14 cases. If it doesn't it should be assumed that the word found does not correspond to the keyword. 
    int i = 0;
    //We get the first word from the sql string and put it in capital letters
    while (*sql != ' ') {
      first_word[i] = toupper(*sql);
      i++;
      sql++;
    }
    first_word[i] = '\0';
    i = 0;
    //We also put the keyword in capital letters for safe measures
    while (*keyword != '\0') {
      word[i] = toupper(*keyword);
      i++;
      keyword++;
    }
    word[i] = '\0';
    if (strcmp(word, first_word) == 0) {
      sql++;
      after_keyword = sql;
    }

    return after_keyword;
}

/*!
*@brief Function extracts a field name and changes pointer position on to the character following the extracted field name
*@param Pointer on a specific position in a string
*@param Buffer containing the extracted value
*@return Pointer on the character following the value we've gathered
*/
char *get_field_name(char *sql, char *field_name) {
  char *after_field_name = NULL;
  while (*sql != ' ' && *sql != '\0') {
    *field_name = *sql;
    sql++;
    field_name++;
  }
  after_field_name = sql;
  return after_field_name;
}

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
    if (*sql == '\0') {
        end = true;
    }
    return end;
}

/*!
*@brief Function extracts a list of fields or values (this type of list is seperated by commas)
*@param Pointer on a specific position in a string
*@param Pointer on a structure where we sill stock the list of fields or values we find
*@return Pointer on the next character following the list of fields or values we've found
*/
char *parse_fields_or_values_list(char *sql, table_record_t *result) {
  char *temp = sql;
  get_sep_space(temp);
  if (*temp == '(') {
    get_sep_space_and_char(sql, '(');
  } else if (*temp == '*') {
    //result->fields->field_value ;
  }else{
    printf("ERREUR MAUVAISE SYNTAXE");
  }

  return sql;
}

/*!
*@brief Function extracts a list of fields (this type of list is seperated by commas, however the field and field type are seperated by a space)
*@param Pointer on a specific position in a string
*@param Pointer on a structure where we will stock the list of fields and field types
*@return Pointer on the next character following the list of fields
*/
char *parse_create_fields_list(char *sql, table_definition_t *result) {
  char *field_name_or_type;
  char name_or_type[150];
  int i = 0;
  char *temp;
  bool insert = false;
  result->fields_count = 0 ;
  get_sep_space(sql);
  if (*sql == '('){
    sql++;
    while (*sql != ')' && sql != NULL){
      get_sep_space(sql);
      get_field_name(sql, field_name_or_type);
      while (*field_name_or_type != '\0') {
        name_or_type[i] = *field_name_or_type;
      }
      strcpy(result->definitions->column_name, name_or_type);
      get_sep_space(sql);
      get_field_name(sql, field_name_or_type);
      get_sep_space_and_char(sql, ',');
      result->fields_count = result->fields_count + 1;
      insert = true;
    }
  }else{
    printf("ERREUR MAUVAISE SYNTAXE");
  }
    return sql;
}

/*!
*@brief Function exctract equality
*@param 
*@param 
*@return Pointer on the next character following the equality  
*/
char *parse_equality(char *sql, field_record_t *equality) {
    return sql;
}

char *parse_set_clause(char *sql, table_record_t *result) {
    return sql;
}

char *parse_where_clause(char *sql, filter_t *filter) {
    return sql;
}

query_result_t *parse(char *sql, query_result_t *result) {
  
    return NULL;
}

query_result_t *parse_select(char *sql, query_result_t *result) {
    return NULL;
}

query_result_t *parse_create(char *sql, query_result_t *result) {
    return NULL;
}

query_result_t *parse_insert(char *sql, query_result_t *result) {
    return NULL;
}

query_result_t *parse_update(char *sql, query_result_t *result) {
    return NULL;
}

query_result_t *parse_delete(char *sql, query_result_t *result) {
    return NULL;
}

query_result_t *parse_drop_db(char *sql, query_result_t *result) {
    return NULL;
}

query_result_t *parse_drop_table(char *sql, query_result_t *result) {
    return NULL;
}
