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
    bool help = false;
    bool probleme_option = false;
    char *path = NULL;
    char *db_name = NULL;
    // Here: check parameters with getopt
    
    while ((opt = getopt(argc, argv, ":hl:d:")) != -1) { 
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
                printf("L'option a besoin d'une valeur\n"); 
                probleme_option = true;
                break;
            case 'h':
                help = true;
                break;
            case '?':
                printf("Option inconnue: %c\n", optopt);
                probleme_option = true;
                break;
        }
    }
    if (help || probleme_option || db_name == NULL) {
        if (help) {
            printf(
                "Options disponibles:\n"
                "-d: Option requis: définie le nom de la base de donnee a administrer\n"
                "-l: Option optionnel: défini le repertoire ou est stocke la base de donnees\n"
                "-h: affiche ce menu\n");
        } else {
            if (probleme_option == false) {
                printf("Vous devez entrer le nom d'une base de donnees (avec l'option -d)\n");
            }
            printf("Pour plus d'information utiliser l'option -h\n");
        }

        if (path != NULL) {
            free(path);
        }
        if (db_name != NULL) {
            free(db_name);
        }
        return EXIT_SUCCESS;
    }

    if (path != NULL) {
        if (directory_exists(path) == false) {
            printf(
                "Le repertoire %s n'existe pas\n"
                "Voulez-vous le creer ?\n"
                "1) Oui\n"
                "2) Non\n"
                "Choix:", path);
            scanf("%d", &tmp);
            printf("\n");
            fflush(stdin);
            while ((tmp != 1) && (tmp != 2)) {
                printf("Vous devez utiliser 1 ou 2\n");
                printf(
                    "Voulez-vous le creer ?\n"
                    "1) Oui\n"
                    "2) Non\n"
                    "Choix:");
                scanf("%d", &tmp);
                printf("\n");
                fflush(stdin);
            }
            if (tmp == 1) {
                _mkdir(path);
            } else {
                return EXIT_FAILURE;
            }
            //dernier teste pour s'assurer que le chemin existe desormais
            if (directory_exists(path) == false) {
                return EXIT_FAILURE;
            }
        }
    }
    //on se place dans le repertoire donnee
    chdir(path);
    /*
    * on cree la base de donnees vide si elle n'existe pas et on se place dedans
    * Durant tout le programme, le repertoire courant sera celui de la base de donnees
    */
    create_db_directory(db_name);
    chdir(db_name);

    //a supprimer
    if (directory_exists("table_2")) {
        recursive_rmdir("table_2");
    }

    char buffer[SQL_COMMAND_MAX_SIZE];
    //allocation de la mémoire dynamiquement
    query_result_t *commande = malloc(sizeof(query_result_t));
    //sauvegarde de la zone mémoire allouée
    query_result_t *temp = commande;
    
    printf("\nAttention: Le code fourni n'est pas terminé, seul quelques fonctions ont été implémentées mais ne suiffisent pas a faire fonctionner l'ensemble du programme\n");

    do {
        
        printf("> ");
        fflush(stdin);
        if (fgets(buffer, SQL_COMMAND_MAX_SIZE, stdin) == NULL)
            continue;
        buffer[strlen(buffer)-1] = '\0';
        if (strcmp(buffer, "exit") == 0)
            break;
        // Here: parse SQL, check query, execute query
        if (strcmp(buffer, "\0") == 0) {
            printf("Aucune commande entrée\n");
        } else {
            commande = parse(buffer, commande);

            if (commande == NULL) {
                commande = temp;
                printf("Commande non acceptée\n");
                continue;
            } else {
                printf("\nCommande interpretee\n");

                if (check_query(commande)) {
                    expand(commande);
                    execute(commande);
                } else {
                    printf("La requete ne peut pas etre executee\n");
                }
            }
        }
        
        
    } while (true);
    free(commande);
    if (path != NULL) {
        free(path);
    }
    if (db_name != NULL) {
        free(db_name);
    }
    return 0;
}