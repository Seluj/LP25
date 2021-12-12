#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include "utils.h"
#include "database.h"
#include "sql.h"
#include "table.h"
#include "check.h"

#define SQL_COMMAND_MAX_SIZE 1500

int main(int argc, char *argv[]) {
    int opt;
    // Here: check parameters with getopt
    
    while ((opt = getopt(argc, argv, ":l:d:")) != -1) { 
        switch (opt) { 
            case 'l': 
                printf("Directory: %s\n", optarg); 
                break; 
            case 'd': 
                printf("Database name: %s\n", optarg); 
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
