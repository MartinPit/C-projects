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

    if (strcmp(path_to_print, ".")) { //* Don't print a new line if it's the first record
        fprintf(file, "\n");
        fprintf(file, "\n");
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
    fprintf(file, "other::%c%c%c", perms[6], perms[7], perms[8]);

    fclose(file);
    return 0;
}

void free_dirent(struct dirent **dirs, int amount)
{
    for (int i = 0; i < amount; i++) {
        free(dirs[i]);
    }

    free(dirs);
}

int is_dir(const struct dirent *file)
{   
    return file -> d_type == DT_DIR;
}

int is_file(const struct dirent *file)
{   
    return file -> d_type == DT_REG;
}

int is_bad(const struct dirent *file)
{
    return !is_file(file) && !is_dir(file);
}

char* get_new_name(char* path, char* name)
{
    char *new_name = malloc(strlen(path) + strlen(name) + 2);
        if (new_name == NULL) {
            perror("Malloc failure.");
            return NULL;
        }

    if (! strcmp("", path)) {
            strcpy(new_name, name);
    } else {
            sprintf(new_name, "%s/%s", path, name); //* adds the name of the folder/file to the path
    }

    return new_name;
}

int export_perms(char* path, char* save_file, char* path_to_print)
{
    struct dirent **dirs = NULL;
    int dir_amount = scandir(path, &dirs, is_dir, alphasort);

    if (dir_amount == -1) {
        perror("Could not scan files in subdirectory.");
        return 1;
    }

    int i;
    for (i = 0; i < dir_amount; i++) {
        char *name = dirs[i] -> d_name;
        if (! strcmp(".", name) || ! strcmp("..", name)) {
            continue;
        }

        char* next = get_new_name(path, name);
        if (next == NULL) {
            perror("Failed to allocate memory.");
            free_dirent(dirs, dir_amount);
            return 1;
        }
        char* new_name = get_new_name(path_to_print, name);
        if (new_name == NULL) {
            perror("Failed to allocate memory.");
            free_dirent(dirs, dir_amount);
            free(next);
            return 1;
        }

        if (save_perms(next, save_file, new_name) == 1) {
            free_dirent(dirs, dir_amount);
            free(next);
            free(new_name);
            return 1;
        }

        if (export_perms(next, save_file, new_name) == 1) {
            free_dirent(dirs, dir_amount);
            free(next);
            free(new_name);
            return 1;
        }

        free(next);
        free(new_name);
    }
    free_dirent(dirs, dir_amount);

    struct dirent **files = NULL;
    int file_amount = scandir(path, &files, is_file, alphasort);

    if (file_amount == -1) {
        perror("Could not scan files in subdirectory.");
        return 1;
    }

    for (i = 0; i < file_amount; i++) {
        char *name = files[i] -> d_name;
        
        char* next = get_new_name(path, name);
        if (next == NULL) {
            perror("Failed to allocate memory.");
            free_dirent(files, file_amount);
            return 1;
        }
        char* new_name = get_new_name(path_to_print, name);
        if (new_name == NULL) {
            perror("Failed to allocate memory.");
            free_dirent(files, file_amount);
            free(next);
            return 1;
        }

        if (save_perms(next, save_file, new_name) == 1) {
            free_dirent(files, file_amount);
            free(next);
            free(new_name);
            return 1;
        }

        free(next);
        free(new_name);
    }
    free_dirent(files, file_amount);

    struct dirent **error_files = NULL;
    int amount = scandir(path, &error_files, is_bad, alphasort);

    if (amount != 0) {
        fprintf(stderr, "File: %s - not regular file or directory\n", error_files[0] -> d_name);
        return 1;
    }

    free_dirent(error_files, amount);
    return 0;
}