#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Doubly Linked List
struct Dllist {
    char* text;
    struct Dllist* next;
    struct Dllist* prev;
};

// Function to add a node to the end of the list chain
struct Dllist* pushNewNode(char* text, ssize_t textLen, struct Dllist* tail) {
    // Null checking
    if (tail == NULL || text == NULL) {
        fprintf(stderr, "Null elements in pushNewNode\n");
        exit(1);
    }

    // Allocate for list node
    struct Dllist* newNode = (struct Dllist*) calloc(1, sizeof(struct Dllist));
    if (newNode == NULL) { // error check
        perror("pushNewNode node");
        exit(1);
    }

    // Dynamically allocates memory given by the readline()
    char* newText = (char*) calloc((textLen +1), sizeof(char));
    if (newText == NULL) { // error check
        perror("pushNewNode text");
        exit(1);
    }
    // Copy the given text to allocation
    strncpy(newText, text, textLen);

    newNode->text = newText; // new text to text
    newNode->prev = tail; // previous = old tail node
    newNode->next = NULL;

    tail->next = newNode; // next of previous = new node
    return newNode;
}

// Function for the initial node
struct Dllist* initialNode(char* text, ssize_t textLen) {
    // Dynamically allocates memory given by the readline()
    struct Dllist* newNode = (struct Dllist*) calloc(1, sizeof(struct Dllist));
    if (newNode == NULL) { // error check
        perror("pushNewNode node");
        exit(1);
    }

    // Allocate for the new text
    char* newText = (char*) calloc((textLen +1), sizeof(char));
    if (newText == NULL) { // error check
        perror("pushNewNode data");
        exit(1);
    }
    // Copy the given text to allocation
    strncpy(newText, text, textLen);

    newNode->text = newText; // new text to text
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
}

// Free the list starting from head
int freeFromHead(struct Dllist* head) {
    struct Dllist* current = head;
    struct Dllist* previous = NULL;

    if (current == NULL) {
        fprintf(stderr, "Null pointer in freeFromHead()");
        exit(1);
    }

    // Going through the list
    while (current->next != NULL)
    {
        previous = current; // Previous is current
        current = current->next; // current is next one
        free(previous->text);
        free(previous);
    }

    free(current->text);
    free(current);
    return 0;
}

// Print starting from tail to head
int printListReverseOrder(FILE* fp, struct Dllist** tail) {
    if (*tail == NULL) {
        fprintf(stderr, "Tail is null in printList\n");
        exit(1);
    }
    
    // Going through the list
    struct Dllist* current = *tail;
    while (current != NULL)
    {
        // Prints to output stream
        fprintf(fp, "%s", current->text);
        current = current->prev;
    }
    return 0;
}

// Read from given fp and store to doubly linked list
int readFromSource(FILE* fp, struct Dllist** head, struct Dllist** tail) {
    // maximum length for a line
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    int lineCnt = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        // Reads from input
        // Read size is used for dynamical allocation

        // If head is null the list does not exist yet
        if (*head == NULL) {
            *head = initialNode(line, read);
            *tail = *head;
        } else {
            // List exists -> push new node to tail
            *tail = pushNewNode(line, read, *tail);
        }
        lineCnt++;
    }

    // Free the line pointer
    free(line);

    // Test if any lines were read
    if (lineCnt > 0)
        printf("Read %d lines\n", lineCnt);
    else {
        fprintf(stderr, "Empty file\n");
        exit(1);
    }
    
    return 0;
}






int main(int argc, char *argv[]) {

    // doubly linked list head and tail
    struct Dllist* listHead = NULL;
    struct Dllist* listTail = NULL;

    // IN and OUT are stdin and stdout by default
    FILE* f_in = stdin;
    FILE* f_out = stdout;

    // Default filenames
    char* in_filename   = "stdin";
    char* out_filename  = "stdout";

    if (argc > 3) { // Too many arguments
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    // How many arguments were given
    switch (argc) {
        case 1: // Neither IN or OUT is given

            // f_in and f_out stay as defaults
            break;
        
        case 2: // IN is given -> replace f_in and in_filename

            in_filename = argv[1];

            // IN file
            if ((f_in = fopen(in_filename, "r")) == NULL) {
                fprintf(stderr, "error: cannot open file '%s'\n", in_filename);
                exit(1);
            }
            break;

        case 3: // IN and OUT are given -> replace both IN and OUT

            in_filename = argv[1];
            out_filename = argv[2];

            // Check for identical input and output
            if (strncmp(in_filename, out_filename, strlen(in_filename)) == 0) {
                fprintf(stderr, "Input and output file must differ\n");
                exit(1);
            }

            // IN file
            if ((f_in = fopen(in_filename, "r")) == NULL) {
                fprintf(stderr, "error: cannot open file '%s'\n", in_filename);
                exit(1);
            }
            // OUT file
            if ((f_out = fopen(out_filename, "w")) == NULL) {
                fprintf(stderr, "error: cannot open file '%s'\n", out_filename);
                exit(1);
            }
            break;
    }

    printf("Reading %s\n", in_filename);
    // Reading from source and adding to doubly linked list
    readFromSource(f_in, &listHead, &listTail);

    printf("Writing %s\n", out_filename);
    // Print to source from doubly linked list
    printListReverseOrder(f_out, &listTail);

    // Free memory
    freeFromHead(listHead);


    /* "Since after the call to fclose() any use of stream results in undefined behavior,
    fclose() should not be used on stdin, stdout, or stderr..."
    - https://pubs.opengroup.org/onlinepubs/9699919799/functions/fclose.html*/
    if (f_in != stdin) // Closing the IN file
        fclose(f_in);
    
    if (f_out != stdout) // Closing the OUT file
        fclose(f_out);
    
    printf("Done! Exiting.\n");
    
    return 0;
}
