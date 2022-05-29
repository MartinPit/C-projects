#ifndef EXPORT_PERMS_H
#define EXPORT_PERMS_H

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

/**
 * @brief Prints the info about the file into the other file given.
 * 
 * @param path info of the file to save
 * @param save_file into what file to save it
 * @param path_to_print what path should represent the saved file
 * @return int 1 if any error occured, 0 otherwise
 */
int save_perms(char* path, FILE* save_file, char* path_to_print);

/**
 * @brief Recursively travels all the subdirectories and files
 * of the given directory indicated by the path variable and saves
 * every file's info into the given file using save_perms function.
 * 
 * @param path what directory to traverse through
 * @param save_file to which to save the info
 * @param path_to_print with what path to represent the directory
 * @return int 1 if any error occured, 0 otherwise
 */
int export_perms(char* path, FILE* save_file, char* path_to_print);

#endif