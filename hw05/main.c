#include "export_perms.h"
#include "import_perms.h"

#define UNUSED(x) ((void) x)

int export_perms(char* path, char* save_file)
{
    if (save_perms(path, save_file, ".") == 1) {
        return 1;
    }

    struct dirent **dirs = NULL;
    int dir_amount = scandir(path, &dirs, NULL, alphasort);

    for (int i = 0; i < dir_amount; i++) {
        char* name = dirs[i] -> d_name;

        if (! strcmp(".", name) || ! strcmp("..", name)) {
            free(dirs[i]);
            continue;
        }

        char *next = malloc(strlen(path) + strlen(name) + 2);
        sprintf(next, "%s/%s", path, name);
        struct stat st;

        if (stat(next, &st) == -1) {
            perror("Failed to initialize stat.");
            free_remaining(dirs, dir_amount, i);
            free(dirs);
            return 1;
        }

        if (S_ISDIR(st.st_mode)) {
            traverse_dirs(next, save_file, name);
        } else {
            save_perms(next, save_file, name);
        }

        free(dirs[i]);
    }
    free(dirs);
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 3 || argc > 4) {
        perror("Incorrect amount of arguments.");
        printf("Aguments should be: <-e/-i> <PERMISSIONS_FILE> [DIRECTORY_TO_CHECK]\n");
        return 1;
    }
    char *directory = NULL;
    if (argc < 4) {
        directory = getcwd(NULL, 0);

    } else {
        directory = argv[3];
    }

    char* filepath = argv[2];
    int result;
    printf("%s %s\n", directory, filepath);

    if (strcmp(argv[1], "-e") == 0) {
        result = export_perms(directory, filepath);
    } else if (strcmp(argv[1], "-i") == 0) {
        result = import_perms(directory, filepath);
    } else {
        result = EXIT_FAILURE;
        fprintf(stderr, "Invalid option flag: %s\n", argv[1]);
    }

    if (argc < 4) {
        free(directory);
    }

    return result;
}
