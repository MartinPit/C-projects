#include "export_perms.h"

void get_flags(mode_t mode, char* array)
{
    if (mode & S_ISUID) {
        array[0] = 's';
    }

    if (mode & S_ISGID) {
        array[1] = 's';
    }

    if (mode & __S_ISVTX) {
        array[2] = 't';
    }
}

void get_user_perms(mode_t mode, char* array)
{
    if (mode & S_IRUSR) {
        array[0] = 'r';
    }

    if (mode & S_IWUSR) {
        array[1] = 'w';
    }

    if (mode & S_IXUSR) {
        array[2] = 'x';
    }
}

void get_group_perms(mode_t mode, char* array)
{
    if (mode & S_IRGRP) {
        array[0] = 'r';
    }

    if (mode & S_IWGRP) {
        array[1] = 'w';
    }

    if (mode & S_IXGRP) {
        array[2] = 'x';
    }
}

void get_other_perms(mode_t mode, char* array)
{
    if (mode & S_IROTH) {
        array[0] = 'r';
    }

    if (mode & S_IWOTH) {
        array[1] = 'w';
    }

    if (mode & S_IXOTH) {
        array[2] = 'x';
    }
}

int save_perms(char* path, char* save_file, char* path_to_print)
{
    struct stat st;

    if (stat(path, &st) == -1) {
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

    char flags[4] = {'-', '-', '-'};
    char user_perms[4] = {'-', '-', '-'};
    char group_perms[4] = {'-', '-', '-'};
    char other_perms[4] = {'-', '-', '-'};

    get_flags(st.st_mode, flags);
    get_user_perms(st.st_mode, user_perms);
    get_group_perms(st.st_mode, group_perms);
    get_other_perms(st.st_mode, other_perms);

    fprintf(file, "# file: %s\n", path_to_print);
    fprintf(file, "# owner: %s\n", pw -> pw_name);
    fprintf(file, "# group: %s\n", grp -> gr_name);

    if (flags[0] != '-' ||
        flags[1] != '-' ||
        flags[2] != '-') {
        fprintf(file, "# flags: %3s\n", flags);
    }

    fprintf(file, "user::%s\n", user_perms);
    fprintf(file, "group::%s\n", group_perms);
    fprintf(file, "other::%s\n", other_perms);
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
    if (save_perms(path, save_file, path_to_print) == 1) {
        return 1;
    }

    struct dirent **dirs = NULL;
    int dir_amount = scandir(path, &dirs, NULL, alphasort);

    for (int i = 0; i < dir_amount; i++) {
        char *name = dirs[i] -> d_name;
        if (! strcmp(".", name) || ! strcmp("..", name)) {
            free(dirs[i]);
            continue;
        }

        char *next = malloc(strlen(path) + strlen(name) + 2);
        sprintf(next, "%s/%s", path, name);
        struct stat st;

        char *new_name = malloc(strlen(path_to_print) + strlen(name) + 2);
        sprintf(new_name, "%s/%s", path_to_print, name);

        if (stat(next, &st) == -1) {
            perror("Failed to initialize stat.");
            free_remaining(dirs, dir_amount, i);
            free(dirs);
            return 1;
        }

        if (S_ISDIR(st.st_mode)) {
            traverse_dirs(next, save_file, new_name);
        }
        free(next);
        free(new_name);
        free(dirs[i]);
    }
    free(dirs);

    return 0;
}