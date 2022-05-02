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
    if ((mode & S_IRWXU) == S_IRUSR) {
        array[0] = 'r';
    }

    if ((mode & S_IRWXU) == S_IWUSR) {
        array[1] = 'w';
    }

    if ((mode & S_IRWXU) == S_IXUSR) {
        array[2] = 'x';
    }
}

void get_group_perms(mode_t mode, char* array)
{
    if ((mode & S_IRWXG) == S_IRGRP) {
        array[0] = 'r';
    }

    if ((mode & S_IRWXG) == S_IWGRP) {
        array[1] = 'w';
    }

    if ((mode & S_IRWXG) == S_IXGRP) {
        array[2] = 'x';
    }
}

void get_other_perms(mode_t mode, char* array)
{
    if ((mode & S_IRWXO) == S_IROTH) {
        array[0] = 'r';
    }

    if ((mode & S_IRWXO) == S_IWOTH) {
        array[1] = 'w';
    }

    if ((mode & S_IRWXO) == S_IXOTH) {
        array[2] = 'x';
    }
}

int save_perms(char* path, char* save_file)
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

    char flags[3] = {'-'};
    char user_perms[3] = {'-'};
    char group_perms[3] = {'-'};
    char other_perms[3] = {'-'};

    get_flags(st.st_mode, flags);
    get_user_perms(st.st_mode, user_perms);
    get_group_perms(st.st_mode, group_perms);
    get_other_perms(st.st_mode, other_perms);

    fprintf(file, "# file: %s\n", path);
    fprintf(file, "# owner: %s\n", pw -> pw_name);
    fprintf(file, "# group: %s\n", grp -> gr_name);

    if (user_perms[0] != '-' ||
        user_perms[1] != '-' ||
        user_perms[2] != '-') {
        fprintf(file, "# flags: %3s\n", flags);
    }
    fprintf(file, "user::%c%c%c\n", user_perms[0], user_perms[1], user_perms[2]);
    fprintf(file, "group::%3s\n", group_perms);
    fprintf(file, "other::%3s\n", other_perms);
    fprintf(file, "\n");

    fclose(file);
    return 0;
}