#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[])
{
    char* filename;

    FILE* file;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    

    if (argc > 1) { // At least one argument is given

        for (size_t i = 1; i < argc; i++) {
            
            filename = argv[i];

            if ((file = fopen(filename, "r")) == NULL) {
                fprintf(stderr, "error: cannot open file '%s'\n", filename);
                exit(1);
            }

            while ((read = getline(&line, &len, file)) != -1) {
                printf("%s", line);
            }
        }
    }


    else printf("usage: /my-cat <file 1> <file 2> ...\n");

    return 0;
}
