#include "sample_printer.h"

#include <stdlib.h>
#include <stdio.h>

#define UNUSED(x) ((void) x)

int main(int argc, char** argv) {
    if (argc != 2) {
        // TODO: Parse the -e and -i options.
        fprintf(stderr, "The PERMISSIONS_FILE argument is required.");
        return EXIT_FAILURE;
    }

    FILE* permissions_file = fopen(argv[1], "w");
    if (!permissions_file) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    print_sample_output(permissions_file);

    fclose(permissions_file);

    return EXIT_SUCCESS;
}
