#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// SOURCES
// https://linux.die.net/
// https://man7.org/linux/man-pages/
// for libc funcs


int main(int argc, char * argv[]) {

    char * filename;
    FILE * file;

    int unzip_size = 1;
    char * unzipped;

    int * intP;
    char * charP;

    // intP is pointer for the amount of letter charP
    // example: 10A -> intP = 10 and charP = A
    if ((intP = (int *) calloc(4, sizeof(int))) == NULL) {
        perror("intP");
        exit(-1);
    }

    if ((charP = (char *) calloc(1, sizeof(char))) == NULL) {
        perror("charP");
        exit(-1);
    }

    // unzipped is the final result
    // smaller parts get appended to unzipped
    if ((unzipped = (char *) calloc(unzip_size, sizeof(char))) == NULL) {
        perror("buffer");
        exit(-1);
    }


    if (argc >= 2) { // One or more arguments were given

        // for every file given print its uncompressed contents
        for (size_t idx = 1; idx < argc; idx++) {
        
            filename    = argv[idx];

            if ((file = fopen(filename, "rb")) == NULL) {
                fprintf(stderr, "error: cannot open file '%s'\n", filename);
                exit(1);
            }

            while (1) {
                
                // intP = how many of letter charP
                if (fread(intP, sizeof(int), 4, file) != 4) {
                    break; // EOF
                }

                if (fread(charP, sizeof(char), 1, file) != 1) {
                    break; // EOF
                }


                // Allocate memory as much as there are letters
                char * buffer;
                if ((buffer = (char *) calloc((*intP+1), sizeof(char))) == NULL) {
                    perror("buffer");
                    exit(-1);
                }

                // Fill with the given character charP for intP times
                memset(buffer, *charP, *intP);

                // Increase size
                unzip_size += *intP;

                // Realloc more space to unzipped version
                unzipped = realloc(unzipped, unzip_size);

                // Append buffer to unzipped
                strncat(unzipped, buffer, *intP);

                // Free buffer
                free(buffer);
            }

            // close the file
            fclose(file);
        }



        // print the final result
        printf("%s\n", unzipped);

        // Tidy up
        free(intP);
        free(charP);
        free(unzipped);
    }

    else printf("my-unzip: file1 [file2 ...]\n");

    return 0;
}