#include <getopt.h>
#include <limits.h>
#include "export_perms.h"
#include "import_perms.h"

int export(char* path, char* save_file)
{   

    FILE* file = fopen(save_file, "w");
    if (file == NULL) {
        perror("File couldn't be opened.");
        return 1;
    }

    if (save_perms(path, file, ".") == 1) { //* print the first folder in a special way
        fclose(file);                       //* with "." as a path to show
        return 1;
    }

    int result = export_perms(path, file, "");
    fclose(file);

    return result;
}

int import(char* path, char* info_file)
{
    FILE* file = fopen(info_file, "r");
    if (file == NULL) {
        perror("File couldn't be opened.");
        return 1;
    }
    int result = import_perms(path, file);

    fclose(file);
    return result;
}

int main(int argc, char** argv) {
     static struct option longopts[] = {
             { "export", required_argument, NULL, 'e' },
             { "import", required_argument, NULL, 'i' },
             { 0 }
     };

    int option;
    if ((option = getopt_long(argc, argv, ":e:i:", longopts, NULL)) == -1) {
       perror("No arguments given.");
       printf("Arguments should be: <-e/-i> <PERMISSIONS_FILE> [DIRECTORY_TO_CHECK]");
       return EXIT_FAILURE;
    }

    int status = 1;
    char cwd[PATH_MAX] = {0};
    getcwd(cwd, sizeof(cwd));

    switch(option){
        case 'i':
            if (argv[optind] == NULL) {
                status = import(cwd, optarg);
            } else {
                status = import(argv[optind], optarg);
            }
            break;
        case 'e':
            if (argv[optind] == NULL) {
                status = export(cwd, optarg);
            } else {
                status = export(argv[optind], optarg);
            }
            break;
        case '?':
            fprintf(stderr, "Unknown option: %c\n", optopt);
            return EXIT_FAILURE;
        case ':':
            fprintf(stderr, "Missing specification for file after %c\n", optopt);
            return EXIT_FAILURE;
        }
    return status;
}
