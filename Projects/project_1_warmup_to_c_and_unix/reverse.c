#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 512


// Doubly Linked List

struct Dllist {
    char* text;
    struct Dllist* next;
    struct Dllist* prev;
};

struct Dllist* listHead;
struct Dllist* listTail;

// Function to add a node to the end of the list chain
struct Dllist* pushNewNode(char* text, struct Dllist* tail) {

    if (tail == NULL || text == NULL) {
        printf("Null elements in pushNewNode\n");
        exit(1);
    }

    // Allocate for list node
    struct Dllist* newNode = (struct Dllist*) malloc(sizeof(struct Dllist));
    if (newNode == NULL) { // error check
        perror("pushNewNode node");
        exit(1);
    }

    // Allocate for the new text
    char* newText = (char*) malloc(sizeof(char) * MAX_LINE_LEN);
    if (newText == NULL) {
        perror("pushNewNode data");
        exit(1);
    }
    strcpy(newText, text); // Copy the given text to allocation

    newNode->text = newText; // new text to text
    newNode->prev = tail; // previous = old tail node
    newNode->next = NULL;

    tail->next = newNode; // next of previous = new node
    return newNode;
}

struct Dllist* initialNode(char* text) {

    // Allocate for list node
    struct Dllist* newNode = (struct Dllist*) malloc(sizeof(struct Dllist));
    if (newNode == NULL) { // error check
        perror("pushNewNode node");
        exit(1);
    }

    // Allocate for the new text
    char* newText = (char*) malloc(sizeof(char) * MAX_LINE_LEN);
    if (newText == NULL) {
        perror("pushNewNode data");
        exit(1);
    }
    strcpy(newText, text); // Copy the given text to allocation

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
        printf("Null pointer in freeFromHead()");
    }

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
int printListReverseOrder(struct Dllist* tail) {
    if (tail == NULL) {
        printf("Null elements in pushNewNode\n");
        exit(1);
    }

    printf("Elements in reverse order: \n");
    
    struct Dllist* current = tail;
    while (current != NULL)
    {
        printf("%s", current->text); // Prints to output stream
        current = current->prev;
    }
    return 0;
}


int readFromSource(FILE* fp) {

    char line[MAX_LINE_LEN];

    int lineCnt = 1;
    while (fgets(line, MAX_LINE_LEN, fp) != NULL) {

        if (listHead == NULL) {
            printf("Node init\n");
            listHead = initialNode(line);
            listTail = listHead;
        } else {
            listTail = pushNewNode(line, listTail);
        }

        lineCnt++;
    }
    fclose(fp);

    return 0;
}






int main(int argc, char const *argv[]) {

    printf("argc: %d\n", argc)

    if (argc > 2) {
        printf("reverse <input> <output>");
        exit(1);
    }

    if (argc == 1) {
        
    }

    printListReverseOrder(listTail);
    
    
    return 0;
}
