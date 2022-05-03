#include "export_perms.h"

void get_flags(mode_t mode, char* array)
{
    array[0] = (mode & S_ISUID) ? 's' : '-';
    array[1] = (mode & S_ISGID) ? 's' : '-';
    array[2] = (mode & __S_ISVTX) ? 't' : '-';
}

void get_perms(mode_t mode, char* array)
{
    array[0] = (mode & S_IRUSR) ? 'r' : '-';
    array[1] = (mode & S_IWUSR) ? 'w' : '-';
    array[2] = (mode & S_IXUSR) ? 'x' : '-';

    array[3] = (mode & S_IRGRP) ? 'r' : '-';
    array[4] = (mode & S_IWGRP) ? 'w' : '-';
    array[5] = (mode & S_IXGRP) ? 'x' : '-';

    array[6] = (mode & S_IROTH) ? 'r' : '-';
    array[7] = (mode & S_IWOTH) ? 'w' : '-';
    array[8] = (mode & S_IXOTH) ? 'x' : '-';
}

int save_perms(char* path, char* save_file, char* path_to_print)
{
    struct stat st;

    if (lstat(path, &st) == -1) {
        perror("Could not get info about file with stat.");
        return 1;
    }

    struct passwd *pw = getpwuid(st.st_uid);

    if (pw == NULL) {
        perror("Could not get info about user with getpwuid.");
        return 1;
    }

    struct group *grp = getgrgid(st.st_gid);

    if (grp == NULL) {
        perror("Could not get info about group with getgrgid.");
        return 1;
    }

    FILE* file = fopen(save_file, "a");

    if (file == NULL) {
        perror("Could not open file.");
        return 1;
    }

    char flags[3];
    char perms[9];

    get_flags(st.st_mode, flags);
    get_perms(st.st_mode, perms);

    fprintf(file, "# file: %s\n", path_to_print);
    fprintf(file, "# owner: %s\n", pw -> pw_name);
    fprintf(file, "# group: %s\n", grp -> gr_name);

    if (flags[0] != '-' || flags[1] != '-' || flags[2] != '-') { //* if no flag is used
        fprintf(file, "# flags: %c%c%c\n", flags[0], flags[1], flags[2]);
    }

    fprintf(file, "user::%c%c%c\n", perms[0], perms[1], perms[2]);
    fprintf(file, "group::%c%c%c\n", perms[3], perms[4], perms[5]);
    fprintf(file, "other::%c%c%c\n", perms[6], perms[7], perms[8]);
    fprintf(file, "\n");

    fclose(file);
    return 0;
}

void free_remaining(struct dirent **dirs, int amount, int position)
{
    for (int i = position; i < amount; i++) {
        free(dirs[i]);
    }
}

int traverse_dirs(char* path, char* save_file, char* path_to_print)
{
    struct dirent **dirs = NULL;
    int dir_amount = scandir(path, &dirs, NULL, alphasort);

    if (dir_amount == -1) {
        perror("Could not scan files in subdirectory.");
        return 1;
    }

    int i;
    for (i = 0; i < dir_amount; i++) {
        char *name = dirs[i] -> d_name;
        if (! strcmp(".", name) || ! strcmp("..", name)) {
            free(dirs[i]);
            continue;
        }

        char *next = malloc(strlen(path) + strlen(name) + 2);
        if (next == NULL) {
            perror("Malloc failure.");
            goto error_handling;
        }

        sprintf(next, "%s/%s", path, name); //* adds the name of the folder/file to the path
        struct stat st;

        char *new_name = malloc(strlen(path_to_print) + strlen(name) + 2);
        if (new_name == NULL) {
            perror("Malloc failure.");
            goto error_handling;
        }

        if (! strcmp("", path_to_print)) {
            strcpy(new_name, name);
        } else {
            sprintf(new_name, "%s/%s", path_to_print, name);
        }

        if (lstat(next, &st) == -1) {
            perror("Failed to initialize stat.");
            goto error_handling;
        }

        if (save_perms(next, save_file, new_name) == 1) {
            goto error_handling;
        }

        if (S_ISDIR(st.st_mode)) {
            if (traverse_dirs(next, save_file, new_name) == 1) {
                goto error_handling;
            }
        }

        free(next);
        free(new_name);
        free(dirs[i]);
    }
    free(dirs);

    return 0;

error_handling:
    free_remaining(dirs, dir_amount, i);
    free(dirs);
    return 1;

}