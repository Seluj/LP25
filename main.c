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
    //full_path = make_full_path(path, db_name);
    //printf("\nPath of the database: %s\n", full_path);
    
    printf("%d", chdir(path));
    create_db_directory(db_name);
    printf("%d", chdir(db_name));
    
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
        /*
        commande = parse(buffer, commande);
        if (check_query(commande)) {
            execute
        }
        */
    } while (true);
    free(full_path);
    free(path);
    free(db_name);
    return 0;
}
