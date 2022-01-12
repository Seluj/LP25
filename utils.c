//
// Created by flassabe on 16/11/2021.
//

#include "utils.h"

#include <dirent.h>

/**
 *  make_full_path concatenates path and basename and returns the result
 *  @param path the path to the database directory basename, can be NULL (i.e. path is current directory)
 *  Path may end with '/' but it is not required.
 *  @param basename the database name.
 *  @return a pointer to the full path. Its content must be freed by make_full_path caller.
 */
char *make_full_path(char *path, char *basename) {
    char *full_path;
    if (path == NULL) {
        full_path = malloc(sizeof(char) * strlen(basename));
        strcpy(full_path, basename);
    } else {
        full_path = malloc(sizeof(char) * (strlen(path) + strlen(basename) + 1));
        strcpy(full_path, path);
        if (path[strlen(path)-1] != '/') {
            strcat(full_path, "/");
        }
        strcat(full_path, basename);
        strcat(full_path, "/");
    }
    return full_path;
}

/**
* Checks if the directory exists
* @param *path - a string containing the directory name
* @return true if the directory exists and false if it doesn't exist
*/
bool directory_exists(char *path) {
    DIR *my_dir = opendir(path);
    if (my_dir) {
        closedir(my_dir);
        return true;
    }
    return false;
}

/**
* Recursive make directory
* @param *dir - a string containing the directory name to be Create
*/
void _mkdir(const char *dir) {
    char tmp[256];
    char *p = NULL;
    size_t len;
    snprintf(tmp, sizeof(tmp), "%s", dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }
    for (p=tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    }
    mkdir(tmp, S_IRWXU);
}