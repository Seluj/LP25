//
// Created by flassabe on 22/11/2021.
//

#include "query_exec.h"

#include <dirent.h>
#include <unistd.h>

#include "database.h"
#include "table.h"

#include <stdlib.h>



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

void execute_create(create_query_t *query){
    create_table(query);
}


void execute_insert(insert_query_t *query) {
    add_row_to_table(query->table_name, &query->fields_names);
}

void execute_select(update_or_select_query_t *query) {
    printf("\n================ TEST ================\n\n");
    printf("Table name :%s\n", query->table_name);
    printf("Il y a %d champs\n", query->set_clause.fields_count);
    printf("Il y a %d valeurs\n", query->where_clause.values.fields_count);
    int untrucaupif=0;
    printf("Champs->type->valeurs:\n");
    while (untrucaupif< query->set_clause.fields_count) {
        printf("\t%d :%s->%d->", untrucaupif, query->set_clause.fields[untrucaupif].column_name, query->set_clause.fields[untrucaupif].field_type);
        switch (query->set_clause.fields[untrucaupif].field_type) {
            case 1:
                printf("%d\n", query->set_clause.fields[untrucaupif].field_value.primary_key_value);
                break;
            case 2:
                printf("%d\n", query->set_clause.fields[untrucaupif].field_value.int_value);
                break;
            case 3:
                printf("%f\n", query->set_clause.fields[untrucaupif].field_value.float_value);
                break;
            default:
                printf("%s\n", query->set_clause.fields[untrucaupif].field_value.text_value);
                break;
        }
        untrucaupif++;
    }
    printf("\n============== FIN TEST ==============\n\n");
    /*
  FILE *content_file;

  content_file = open_definition_file(content_definition->table_name, "r");   
  if(content_file!=NULL){  

    add_row_to_table(query->table_name, &query->fields_names);
}

void execute_select(update_or_select_query_t *query) {
    /*
  FILE *content_file;

  content_file = open_definition_file(content_definition->table_name, "r");   
  if(content_file!=NULL){  
      display_table_record_list(update_or_select_query_t.table_name);
  }
  else  {
    printf("Fichier introuvable.");
    exit(1);
  }
  fclose(content_file);
  */
}

void execute_update(update_or_select_query_t *query) {
  FILE *data_file = open_content_file(query->table_name, "wb");
    if (query->where_clause.values.fields_count == 0) {
      
    } else{     
     
   }   
}



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
        /*on ouvre le fichier d'index'et on place le curseur sur le numéro de ligne de la table que nous voulons supprimer. 
        On récupère ensuite cette valeur et on lui soustrait 1000000000000 afin de rendre
        le champ inactif.*/
      /*
        FILE *data_file = open_content_file(query->table_name, "rb");
        FILE *index_file = open_index_file(query->table_name, "r+b");
        numero_de_ligne= numero2ligne(query->table_name);
        fseek(index_file,,SEEK_SET);
        fread(position, sizeof(long long), 1, index_file); 
        position=position-1000000000000000000000000000000000000000000000000;               
        fprintf(index_file, %lli, position);
        fwrite(&position, sizeof(long long), 1, index_file);
        */
    }
}

int numero2ligne(char *table_name){
/*retourne le numéro de la ligne du champs que l'on doit supprimer
int length=compute_record_length(query->table_name);*/
}

void execute_drop_table(char *table_name) {
    if (directory_exists(table_name)) {
        drop_table(table_name);
    }
}

void execute_drop_database(char *db_name) {
    chdir("..");
    recursive_rmdir(db_name);
    exit(EXIT_SUCCESS);
}
