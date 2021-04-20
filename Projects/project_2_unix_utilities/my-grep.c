#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[])
{
    char* searchTerm;
    char* filename;

    FILE* file;

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    char* token;

    if (argc == 3) { // Two arguments are given

        searchTerm  = argv[1];
        filename    = argv[2];

        if ((file = fopen(filename, "r")) == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", filename);
            exit(1);
        }

        while ((read = getline(&line, &len, file)) != -1) {
            token = strstr(line, searchTerm);
            if (token != NULL) {
                printf("%s", line);
            }
        }
    }

    else printf("usage: /my-grep <search term> <filename>\n");

    return 0;
}
