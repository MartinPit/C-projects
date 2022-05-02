#include "export_perms.h"
#include "import_perms.h"

#define UNUSED(x) ((void) x)

int export_perms(char* directory, char* filepath)
{
    save_perms(directory, filepath);

    return 0;
}

int main(int argc, char** argv) {
    if (argc < 3 || argc > 4) {
        perror("Incorrect amount of arguments.");
        printf("Aguments should be: <-e/-i> <PERMISSIONS_FILE> [DIRECTORY_TO_CHECK]\n");
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
