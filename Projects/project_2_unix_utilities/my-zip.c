#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// SOURCES
// https://linux.die.net/
// https://man7.org/linux/man-pages/
// for libc funcs
//
// https://www.learn-c.org/en/Linked_lists
// for linked lists


// Struct for compressed format
// character: A, count: 4 -> 4A -> AAAA
typedef struct node {
    char character;
    int count;
    struct node * next;
} node_t;

// Function for adding to linked lists
int addToLinkedList(node_t * head, char c, int cnt) {
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    // Variables
    if ((current->next = (node_t *) calloc(1, sizeof(node_t))) == NULL) {
        perror("add to linked");
        exit(1);
    }
    current->next->character    = c;
    current->next->count        = cnt;
    current->next->next         = NULL;
    return 0;
}

// function for initalizing linked list
node_t * initLinkedList(char c, int cnt) {

    node_t * head;
    if ((head = (node_t *) calloc(1, sizeof(node_t))) == NULL) {
        perror("init linked");
        exit(1);
    }
    head->character = c;
    head->count     = cnt;
    head->next      = NULL;

    return head;
}

// function for freeing linked list by traversing and cleaning
int freeLinkedList(node_t * head) {
    if (head != NULL) {

        node_t * temp1 = head;
        node_t * temp2 = NULL;

        while ((temp2 = temp1->next) != NULL) {
            free(temp1);
            temp1 = temp2;
        }
        free(temp1);
        free(temp2);
    }
    return 0;
}


int main(int argc, char * argv[]) {

    char * filename;
    FILE * file;

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    // head of the linked list head
    node_t * head = NULL;


    if (argc >= 2) { // One or more arguments were given

        // for every file given compress and write it
        for (size_t idx = 1; idx < argc; idx++) {
        
            filename    = argv[idx];

            // open file
            if ((file = fopen(filename, "r")) == NULL) {
                fprintf(stderr, "error: cannot open file '%s'\n", filename);
                exit(1);
            }

            // prevChar is the char before curChar
            // count is how many of the same char are encountered
            char prevChar   = 0;
            char curChar    = 0;
            int count       = 0;

            // read every line of the file
            while ((read = getline(&line, &len, file)) != -1) {

                // for every character in line
                for (size_t idx = 0; idx < read; idx++) {

                    curChar = line[idx];

                    // for first run
                    if (prevChar == 0) {
                        prevChar = curChar;
                    }

                    // if current is same as prev
                    // increment count by one
                    if (curChar == prevChar) {
                        count++;

                        // If last in line -> add remaining to list
                        if (idx == (read-1)) {
                            goto last;
                        }
                    }
                    
                    // if current is different of previous
                    // add the record and start recording new
                    else {
                        if (head != NULL) {
                            last:
                            addToLinkedList(head, prevChar, count);
                        } else {
                            // first time -> init new head
                            head = initLinkedList(prevChar, count);
                        }
                        
                        count = 1;
                    }

                    // current becomes new previous
                    prevChar = curChar;
                }
            }

            // cleanup allocs and file
            fclose(file);
            free(line);
            
            // head of the list
            node_t * curr = head;

            // countP is how many of charP
            // 10A, 23H, etc.
            int * countP = NULL;
            if ((countP = calloc(4, sizeof(int))) == NULL) {
                perror("countP");
                exit(1);
            }

            char * charP = NULL;
            if ((charP = calloc(1, sizeof(char))) == NULL) {
                perror("charP");
                exit(1);
            }
            
            // traverse linked list and write the content to stdout
            while (curr != NULL)
            {
                * countP = curr->count;
                * charP  = curr->character;
                
                // 4-byte integer and single ACII char
                fwrite(countP, sizeof(int), 4, stdout);
                fwrite(charP, sizeof(char), 1, stdout);
                curr = curr->next;
            }


            // free allocated memory
            free(countP);
            free(charP);

            freeLinkedList(head);

            line = NULL;
            head = NULL;
        }
    }

    else printf("my-zip: file1 [file2 ...]\n");

    return 0;
}