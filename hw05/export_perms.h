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

// typedef struct file_info
// {
//     char* path[_PC_PATH_MAX];
//     char* owner[NAME];
//     char* group[_PC_]
//     char* flags[3];
//     char* user_perms[3];
//     char* group_perms[3];
//     char* other_perms[3];
// } file_info;


int save_perms(char* path, char* save_file);
// int init_info(file_info* info);
// int fill_info(file_info* info,char* directory);

#endif