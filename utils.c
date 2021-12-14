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
    int i = strlen(path);
    if (strcmp(&path[i-1], "/") != 0) {
      strcpy(path, "/");
      strcat(path, basename);
    } else {
      strcat(path, basename);
    }
    return path;
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
