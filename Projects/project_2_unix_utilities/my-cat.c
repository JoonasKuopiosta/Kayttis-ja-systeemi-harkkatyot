#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// SOURCES
// https://linux.die.net/
// https://man7.org/linux/man-pages/
// for fopen and getline

int main(int argc, char *argv[])
{
    char* filename;

    FILE* file;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    

    if (argc > 1) { // At least one argument is given

        // iterate over every argument (file name)
        for (size_t i = 1; i < argc; i++) {
            
            // filename is the given argument
            filename = argv[i];
            // Open the file
            if ((file = fopen(filename, "r")) == NULL) {
                fprintf(stderr, "error: cannot open file '%s'\n", filename);
                exit(1);
            }

            // Write every line to stdout
            while ((read = getline(&line, &len, file)) != -1) {
                printf("%s", line);
            }

            fclose(file);
        }

        // Cleaning up before exiting
        free(line);
    }

    // If no arguments are given just returns 0

    return 0;
}
