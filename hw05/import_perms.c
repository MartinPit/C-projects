#include "import_perms.h"

perm_info* get_info(FILE* file)
{
    perm_info* info = malloc(sizeof(perm_info));
    if (info == NULL) {
        perror("Failed to allocate memory");
        return 1;
    }

    char* line = NULL;
    return 1;
}

int import_perms(char* directory, FILE* info_file) {
    
    perm_info* info = get_info(info_file);
    return 0;
}