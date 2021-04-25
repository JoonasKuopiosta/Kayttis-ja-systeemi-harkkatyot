#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// SOURCES
// https://linux.die.net/
// https://man7.org/linux/man-pages/
// for fopen, getline and strstr


int checkFile(char * filename, char * searchTerm) {

    FILE* file;

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    char* pointer;

    // if filename is NULL use stdin
    if (filename == NULL) {
        file = stdin;

    } else {
        // if filename is given, open it
        if ((file = fopen(filename, "r")) == NULL) {
            fprintf(stderr, "my-grep: cannot open file '%s'\n", filename);
            exit(1);
        }
    }

    // reading every line
    while ((read = getline(&line, &len, file)) != -1) {

        // if pointer is not null, a matching pattern was found
        // -> print the line
        pointer = strstr(line, searchTerm);
        if (pointer != NULL) {
            printf("%s", line);
        }
    }


    /* "Since after the call to fclose() any use of stream results in undefined behavior,
    fclose() should not be used on stdin, stdout, or stderr..."
    - https://pubs.opengroup.org/onlinepubs/9699919799/functions/fclose.html*/
    if (file != stdin) {
        fclose(file);
    }

    // free used memory
    free(line);
    free(pointer);

    return 0;
}


int main(int argc, char *argv[])
{
    char * searchTerm = NULL;
    char * filename = NULL;

    if (argc > 1) { // At least one argument is given

        // search term is always first argument
        searchTerm = argv[1];

        if (argc == 2) { // search term is given but no file
            checkFile(filename, searchTerm);
        
        
        } else {

            // Iterate through all given files
            for (size_t idx = 2; idx < argc; idx++) {
                
                // call the grepping function file at a time
                filename = argv[idx];
                checkFile(filename, searchTerm);
            }
            
        }
    }

    else {
        printf("usage: /my-grep <search term> <filename ...>\n");
        exit(1);
    }

    return 0;
}
