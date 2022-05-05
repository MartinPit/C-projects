#include "export_perms.h"
#include "import_perms.h"
#include <getopt.h>

int export(char* path, char* save_file)
{
    if (save_perms(path, save_file, ".") == 1) { //* print the first folder in a special way
                                                 //* with "." as a path to show
        return 1;
    }

    return export_perms(path, save_file, "");
}

int import(char* path, char* info_file)
{
    FILE* file = fopen(info_file, "r");
    int result = import_perms(path, file);

    fclose(file);
    return result;
}

int main(int argc, char** argv) {
     static struct option longopts[] = {
             { "export", required_argument, NULL, 'e' },
             { "import", required_argument, NULL, 'i' },
             { 0, 0, 0, 0 }
     };

    int option;
    if ((option = getopt_long(argc, argv, ":e:i:", longopts, NULL)) == -1) {
       perror("No arguments given.");
       printf("Arguments should be: <-e/-i> <PERMISSIONS_FILE> [DIRECTORY_TO_CHECK]");
       return EXIT_FAILURE;
    }

    switch(option){
        case 'i':
            if (argv[optind] == NULL) {
                return import(getcwd(NULL, 0), optarg);
            } else {
                return import(argv[optind], optarg);
            }
            break;
        case 'e':
            if (argv[optind] == NULL) {
                return export(getcwd(NULL, 0), optarg);
            } else {
                return export(argv[optind], optarg);
            }
            break;
        case '?':
            fprintf(stderr, "Unknown option: %c\n", optopt);
            return EXIT_FAILURE;
        case ':':
            fprintf(stderr, "Missing specification for file after %c\n", optopt);
            return EXIT_FAILURE;
        }

    return EXIT_SUCCESS;
}
