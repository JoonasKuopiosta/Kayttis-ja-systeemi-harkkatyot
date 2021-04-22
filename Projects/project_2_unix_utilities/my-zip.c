#include <stdio.h>
#include <stdlib.h>
#include <string.h>




typedef struct node {
    char character;
    int count;
    struct node * next;
} node_t;


int addToLinkedList(node_t * head, char c, int cnt) {
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    // Variables
    if ((current->next = (node_t *) calloc(1, sizeof(node_t))) == NULL) {
        perror("add to linked");
        exit(-1);
    }
    current->next->character    = c;
    current->next->count        = cnt;
    current->next->next         = NULL;
}


node_t * initLinkedList(char c, int cnt) {

    node_t * head;
    if ((head = (node_t *) calloc(1, sizeof(node_t))) == NULL) {
        perror("init linked");
        exit(-1);
    }
    head->character = c;
    head->count     = cnt;
    head->next      = NULL;

    return head;
}


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
}


int main(int argc, char * argv[]) {

    char * filename;
    FILE * file;

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    node_t * head = NULL;


    if (argc == 2) { // One argument is given

        filename    = argv[1];

        if ((file = fopen(filename, "r")) == NULL) {
            fprintf(stderr, "error: cannot open file '%s'\n", filename);
            exit(1);
        }

        while ((read = getline(&line, &len, file)) != -1) {

            char prevChar   = 0;
            char curChar    = 0;
            int count       = 0;

            for (size_t idx = 0; idx < read; idx++) {

                curChar = line[idx];

                if (prevChar == 0) {
                    prevChar = curChar;
                }

                if (curChar == prevChar) {
                    count++;

                    // If last in line -> add remaining to list
                    if (idx == (read-1))
                        goto last;
                }
                
                else {
                    if (head != NULL)
                        last:
                        addToLinkedList(head, prevChar, count);
                    else
                        head = initLinkedList(prevChar, count);
                    
                    count = 1;
                }

                prevChar = curChar;
            }
        }

        fclose(file);
        free(line);

        node_t * curr = head;

        int * countP;
        if ((countP = (int *) calloc(1, sizeof(int))) == NULL) {
            perror("countP");
            exit(0);
        }

        char * charP;
        if ((charP = (char *) calloc(1, sizeof(char))) == NULL) {
            perror("charP");
            exit(0);
        }
        
        while (curr != NULL)
        {
            * countP = curr->count;
            * charP  = curr->character;

            fwrite(countP, sizeof(int), 4, stdout);
            fwrite(charP, sizeof(char), 1, stdout);
            curr = curr->next;
        }

        free(countP);
        free(charP);

        freeLinkedList(head);
    }

    else printf("usage: /my-zip <filename>\n");

    return 0;
}