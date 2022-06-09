#ifndef IMPORT_PERMS_H
#define IMPORT_PERMS_H

#ifndef S_ISVTX
#define S_ISVTX 0001000
#endif

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <dirent.h>

typedef struct perm_info {
    char* path;
    char* owner;
    char* group;
    char flags[3];
    char perms[9];

} perm_info;

/**
 * @brief Reads the contents of the given file and changes the permissions
 * for the files specified in it in the given directory.
 * 
 * @param directory directory to traverse
 * @param filepath file from which to read the information
 * @return int 1 if error occured, 0 otherwise
 */
int import_perms(char* directory, FILE* info_file);

#endif