#include "import_perms.h"

perm_info* get_info(FILE* file)
{
    perm_info* info = malloc(sizeof(perm_info));
    if (info == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }

    char* line = NULL;
    size_t aloc_amount = 0;
    ssize_t length = 0;

    length = getline(&line, &aloc_amount, file);
    info->path = malloc(length - 8);
    if (info->path == NULL) {
        perror("Failed to allocate memory");
        free(info);
        return NULL;
    }

    sscanf(line, "# file: %s", info->path);

    length = getline(&line, &aloc_amount, file);
    info->owner = malloc(length - 9);
    if (info->owner == NULL) {
        perror("Failed to allocate memory");
        free(info);
        free(info->path);
        return NULL;
    }

    sscanf(line, "# owner: %s", info->owner);

    length = getline(&line, &aloc_amount, file);
    info->group = malloc(length - 9);
    if (info->group == NULL) {
        perror("Failed to allocate memory");
        free(info);
        free(info->path);
        free(info->owner);
        return NULL;
    }

    sscanf(line, "# group: %s", info->group);

    length = getline(&line, &aloc_amount, file);

    if (line[0] == '#') {
        sscanf(line, "# flags: %c%c%c", &(info->flags[0]), &(info->flags[1]), &(info->flags[2]));
        length = getline(&line, &aloc_amount, file);
    } else {
        info->flags[0] = '-';
        info->flags[1] = '-';
        info->flags[2] = '-';
    }

    sscanf(line, "user::%c%c%c", &(info->perms[0]), &(info->perms[1]), &(info->perms[2]));

    length = getline(&line, &aloc_amount, file);
    sscanf(line, "group::%c%c%c", &(info->perms[3]), &(info->perms[4]), &(info->perms[5]));

    length = getline(&line, &aloc_amount, file);
    sscanf(line, "other::%c%c%c", &(info->perms[6]), &(info->perms[7]), &(info->perms[8]));

    getline(&line, &aloc_amount, file);
    free(line);

    return info;
}

void destroy_info(perm_info* info)
{
    free(info->path);
    free(info->owner);
    free(info->group);
    free(info);
}

void create_mode(mode_t* mode, perm_info* info)
{
    char* flags = info->flags;
    char* perms = info->perms;

    if (flags[0] == 's') {
        *mode |= S_ISUID;
    }

    if (flags[1] == 's') {
        *mode |= S_ISGID;
    }

    if (flags[2] == 't') {
        *mode |= S_ISVTX;
    }

    if (perms[0] == 'r') {
        *mode |= S_IRUSR;
    }

    if (perms[1] == 'w') {
        *mode |= S_IWUSR;
    }

    if (perms[2] == 'x') {
        *mode |= S_IXUSR;
    }

    if (perms[3] == 'r') {
        *mode |= S_IRGRP;
    }

    if (perms[4] == 'w') {
        *mode |= S_IWGRP;
    }

    if (perms[5] == 'x') {
        *mode |= S_IXGRP;
    }

    if (perms[6] == 'r') {
        *mode |= S_IROTH;
    }

    if (perms[7] == 'w') {
        *mode |= S_IWOTH;
    }

    if (perms[8] == 'x') {
        *mode |= S_IXOTH;
    }
}

int set_perms(char* path, perm_info* info)
{
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("Couldn't read info about the file with stat");
        return 0;
    }

    struct passwd *pw = getpwuid(st.st_uid);

    if (pw == NULL) {
        perror("Could not get info about user with getpwuid");
        return 0;
    }

    struct group *grp = getgrgid(st.st_gid);

    if (grp == NULL) {
        perror("Could not get info about group with getgrgid");
        return 0;
    }

    if (strcmp(info->owner, pw->pw_name)) {
        fprintf(stderr, "User of file %s is incorrect\n", path);
    }

    if (strcmp(info->group, grp->gr_name)) {
        fprintf(stderr, "Group of file %s is incorrect\n", path);
    }

    mode_t mode = 0;
    create_mode(&mode, info);

    if (chmod(path, mode) == -1) {
        perror("Couldn't set permissions with chmod");
        return 0;
    }

    return 1;
}

int import_perms(char* directory, FILE* info_file) 
{
    int return_val = 0;
    size_t dir_len = strlen(directory);
    
    while (!feof(info_file)) {
        perm_info* info = get_info(info_file);
        if (info == NULL) {
            return 1;
        }
        char* full_path = malloc(dir_len + strlen(info->path) + 2);
        if (full_path == NULL) {
                perror("Failed to allocate memory.");
                destroy_info(info);
                return 1;
            }

        if (! strcmp(info->path, ".")) {
            strcpy(full_path, directory);
        } else {
            sprintf(full_path, "%s/%s", directory, info->path);
        }

        if (!set_perms(full_path, info)) {
            return_val = 1;
        }

        destroy_info(info);
        free(full_path);
    }


    return return_val;
}