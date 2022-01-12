/*!
 * @file database.c
 * @brief fichier de fonctions de gestion de la base de données
 * @author Jules F.
 * @date 2021 - 2022
 */

#include "database.h"

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>

#include "utils.h"

/*!
 * @brief créer un dossier pour la base de données s'il n'existe pas
 * @param name nom de la base de données
 */
void create_db_directory(char *name) {
    if (!directory_exists(name)) {
        mkdir(name, S_IRWXU);
    }
}

/*
 * @brief supprimer récursivement tous les fichiers et dossiers d'un dossier donné en parametre
 * @param dirname dossier a supprimer
 * @return un entier
 */
int recursive_rmdir(char *dirname) {
    DIR *d = opendir(dirname);
    size_t path_len = strlen(dirname);
    int r = -1;

    if (d) {
        struct dirent *p;

        r = 0;
        while (!r && (p=readdir(d))) {
            int r2 = -1;
            char *buf;
            size_t len;

            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
                continue;
            }
            len = path_len + strlen(p->d_name) + 2; 
            buf = malloc(len);

            if (buf) {
                struct stat statbuf;

                snprintf(buf, len, "%s/%s", dirname, p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode)) {
                        r2 = recursive_rmdir(buf);
                    } else {
                        r2 = unlink(buf);
                    }
                }
                free(buf);
            }
            r = r2;
        }
        closedir(d);
    }
    if (!r) {
        r = rmdir(dirname);
    }
    return r;
}
