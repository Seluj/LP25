#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>


#include "utils.h"
#include "database.h"
#include "sql.h"
#include "table.h"
#include "check.h"
#include "expand.h"
#include "query_exec.h"

#define SQL_COMMAND_MAX_SIZE 1500

int main(int argc, char *argv[]) {
    int tmp, opt;
    char *path = NULL;
    char *db_name = NULL;
    // Here: check parameters with getopt
    
    while ((opt = getopt(argc, argv, ":l:d:")) != -1) { 
        switch (opt) { 
            case 'l':
                path = malloc(sizeof(char) * strlen(optarg));
                strcpy(path, optarg);
                break;
            case 'd':
                db_name = malloc(sizeof(char) * strlen(optarg));
                strcpy(db_name, optarg);
                break;
            case ':':
                printf("option needs a value\n"); 
                return EXIT_FAILURE;
                break;
            case '?':
                printf("unknown option: %c\n", optopt);
                return EXIT_FAILURE;
                break;
        }
    }
    if (db_name == NULL) {
        printf("You must enter a database name (with option -d)\n");
        return EXIT_FAILURE;
    }
    if (path != NULL) {
        if (directory_exists(path) == false) {
            printf("Path doesn't exist\n"
            "Would you want to create one ?\n"
            "1) Yes\n"
            "2) No\n"
            "Choice:");
            scanf("%d", &tmp);
            printf("\n");
            fflush(stdin);
            while ((tmp != 1) && (tmp != 2)) {
                printf("You must write 1 or 2\n");
                printf("Would you want to create one ?\n"
            "1) yes\n"
            "2) no\n"
            "Choice:");
                scanf("%d", &tmp);
                printf("\n");
                fflush(stdin);
            }
            if (tmp == 1) {
                _mkdir(path);
            } else {
                return EXIT_FAILURE;
            }
            
            if (directory_exists(path) == false) {
                return EXIT_FAILURE;
            }
        } else {
            printf("\nPath exist\n");
        }
    }
    chdir(path);
    create_db_directory(db_name);
    chdir(db_name);

/* -------------------------------------- TEST -------------------------------------------- */

    table_definition_t *test1 = malloc(sizeof(table_definition_t));
    test1 = get_table_definition("table_1", test1);
    field_definition_t *test2 = malloc(sizeof(field_definition_t));
    printf("\nTeste de lecture de definition : \n");
    if (test1 != NULL) {
        int i=0;
        printf("\tDefinition of %s: \n", "table_1");
        while (i < test1->fields_count) {
            printf("\t\t%d\t%s\n", test1->definitions[i].column_type, test1->definitions[i].column_name);
            i++;
        }
        printf("\nTeste de find_field_definition : \n");
        test2 = find_field_definition("bonjour", test1);
        if (test2 != NULL) {
            printf("\tDefinition of field called %s: \n", "bonjour");
            printf("\t\t%d\t%s\n", test2->column_type, test2->column_name);
        } else {
            printf("\tField doesn't exist\n");
        }

        printf("\nTeste de check_fields_list : \n");
        table_record_t *test3 = malloc(sizeof(table_record_t));
        test3->fields_count=2;
        strcpy(test3->fields[0].column_name, "bonjour");
        strcpy(test3->fields[1].column_name, "allo");
        if (check_fields_list(test3, test1)) {
            printf("\tTest OK, la liste correspond\n");
        } else {
            printf("\tPROBLEME\n");
        }
        
        printf("\nTeste de check_value_types\n");
        strcpy(test3->fields[0].field_value.text_value, "21");
        strcpy(test3->fields[1].field_value.text_value, "2102");
        printf("\tAvant fonction : \n");
        printf("\t\tChamps 0 :\t%d\t%s\n", test3->fields[0].field_type, test3->fields[0].column_name);
        printf("\t\tChamps 1 :\t%d\t%s\n", test3->fields[1].field_type, test3->fields[1].column_name);
        if (check_value_types(test3, test1)) {
            printf("\tTest OK\n");
        } else {
            printf("\tTest NOT OK\n");
        }
        printf("\tApres fonction : \n");
        printf("\t\tChamps 0 :\t%d\t%s\n", test3->fields[0].field_type, test3->fields[0].column_name);
        printf("\t\tChamps 1 :\t%d\t%s\n", test3->fields[1].field_type, test3->fields[1].column_name);

        printf("\nTeste de tous les query : \n");
        query_result_t *test4
        check_query(query_result_t *query)

    } else {
        printf("Impossible de lire le fichier\n");
    }



/* ------------------------------------ FIN TEST ------------------------------------------ */

    char buffer[SQL_COMMAND_MAX_SIZE];
    query_result_t *commande = malloc(sizeof(query_result_t));
    do {
       printf("> ");
        fflush(stdin);
        if (fgets(buffer, SQL_COMMAND_MAX_SIZE, stdin) == NULL)
            continue;
        buffer[strlen(buffer)-1] = '\0';
        if (strcmp(buffer, "exit") == 0)
            break;
        // Here: parse SQL, check query, execute query
        
        commande = parse(buffer, commande);
        if (commande == NULL) {
            continue;
        } else {
            if (check_query(commande)) {
                execute(commande);
            }
        }
        
        
    } while (true);
    //free(full_path);
    
    free(path);
    free(db_name);
    return 0;
}
