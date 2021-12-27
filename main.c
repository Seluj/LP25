#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "utils.h"
#include "database.h"
#include "sql.h"
#include "table.h"
#include "check.h"

#define SQL_COMMAND_MAX_SIZE 1500

int main(int argc, char *argv[]) {
    int tmp, opt;
    char *path = NULL;
    char *db_name = NULL;
    // Here: check parameters with getopt
    
    while ((opt = getopt(argc, argv, ":l:d:")) != -1) { 
        switch (opt) { 
            case 'l': 
                printf("Directory: %s\n", optarg);
                path = optarg;
                break; 
            case 'd': 
                printf("Database name: %s\n", optarg);
                db_name = optarg;
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
                printf("You must write yes or no\n");
                printf("Would you want to create one ? (yes/no)\n"
            "1) yes\n"
            "2) no\n"
            "Choice:");
                scanf("%d", &tmp);
                printf("\n");
                fflush(stdin);
            }
            if (tmp == 1) {
                mkdir(path);
            } else {
                return EXIT_FAILURE;
            }
            if (directory_exists(path)) {
                return EXIT_FAILURE;
            }
        }
    }
    printf("path: %s\n", path);
    printf("Database name: %s\n", db_name);
    
    char buffer[SQL_COMMAND_MAX_SIZE];
    do {
       printf("> ");
        fflush(stdin);
        if (fgets(buffer, SQL_COMMAND_MAX_SIZE, stdin) == NULL)
            continue;
        buffer[strlen(buffer)-1] = '\0';
        if (strcmp(buffer, "exit") == 0)
            break;
        // Here: parse SQL, check query, execute query
        
    } while (true);
    return 0;
}
