#ifndef EXPORT_PERMS_H
#define EXPORT_PERMS_H

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

int save_perms(char* path, char* save_file, char* path_to_print);
int traverse_dirs(char* path, char* save_file, char* path_to_print);
void free_remaining(struct dirent **dirs, int amount, int position);

#endif