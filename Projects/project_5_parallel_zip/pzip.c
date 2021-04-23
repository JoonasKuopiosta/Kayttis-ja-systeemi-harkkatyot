#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <pthread.h>


// Linked list node struct
typedef struct node {
    char character;
    int count;
    struct node * next;
} node_t;

// To init list and return head
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

// Function to add node to the end of list
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
    return 0;
}

// Function to traverse and free the list
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

// Function to return pointer to the tail of given list
node_t * getTailLinkedList(node_t * head) {
    if (head != NULL) {

        node_t * curr = head;
        // Traverse to the end of list
        while (curr->next != NULL)
        {
            curr = curr->next;
        }
        return curr;
    }
    return NULL;
}






// GLOBALS
// List of list heads
node_t ** head_list = NULL;
// Global head_list_lock
pthread_mutex_t head_list_lock;





// Data strcut for threadFunction
typedef struct th_data_struct {
    char * filename;
    int    index;
} th_data;


void * threadFunction(void * in_data) {

    th_data * data  = (th_data *) in_data;
    char * filename = data->filename;
    int file_num    = data->index;


    FILE * file;

    char * line = NULL;
    size_t len = 0;
    ssize_t read;


    node_t * head = NULL;

    if ((file = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "error: cannot open file '%s'\n", filename);
        exit(1);
    }


    // prevChar is the char before curChar
    // count is how many of the same char are encountered
    char prevChar   = 0;
    char curChar    = 0;
    int count       = 0;

    // Iterate through lines
    while ((read = getline(&line, &len, file)) != -1) {

        // Iterate through characters in line
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
            // current char becomes the previous char
            prevChar = curChar;
        }
    }

    // Free allocations
    fclose(file);
    free(line);

    // get lock before using global list
    pthread_mutex_lock(&head_list_lock);

    head_list[file_num] = head;
    // release the lock after adding list head
    pthread_mutex_unlock(&head_list_lock);

    // free and detach all thread memory
    free(data);
    pthread_detach(pthread_self());
    
    return NULL;
}





int main(int argc, char * argv[]) {

    node_t * main_head = NULL;

    // Number of system available threads
    //int th_count = get_nprocs();

    // Number of files to be read
    int file_count = argc-2;
    int th_count = file_count;

    // List of pointers to thread_id pointers
    pthread_t ** thread_list;
    if ((thread_list = calloc(th_count, sizeof(pthread_t *))) == NULL) {
        perror("thread_list");
        exit(-1);
    }

    // List of pointers to list heads
    if ((head_list = calloc(th_count, sizeof(node_t *))) == NULL) {
        perror("head_list");
        exit(-1);
    }

    // Test if lock works
    if (pthread_mutex_init(&head_list_lock, NULL) != 0) {
        perror("head list lock init");
        exit(-1);
    }



    if (file_count >= 1) { // One or more files are given

        // Pass all given files to threads for processing
        for (size_t idx = 0; idx < file_count; idx++) {

            th_data * data;
            if ((data = calloc(1, sizeof(th_data))) == NULL) {
                perror("data alloc");
                exit(-1);
            }
            data->index = idx;
            data->filename = argv[idx +1];

            pthread_t thread_id;
            if (pthread_create(&thread_id, NULL, threadFunction, (void *) data) != 0) {
                perror("thread create");
                exit(-1);
            }
            thread_list[idx] = &thread_id;

            //printf("Thread sent: %ld\n", idx);
        }


        // Wait for all threads to finish
        for (size_t idx = 0; idx < file_count; idx++) {
            pthread_t * thread = thread_list[idx];
            pthread_join(*thread, NULL);
        }


        // Creating a chain of linked lists
        // 0 -> 1 -> 2 -> ...
        // get the first head as the main head
        main_head = head_list[0];
        node_t * prev = main_head;
        node_t * prev_tail = getTailLinkedList(prev);
        node_t * curr;

        for (size_t idx = 1; idx < file_count; idx++) {
            // Get next in list as current
            curr = head_list[idx];
            // Set the next of the tail in previous one to the first in current
            prev_tail->next = curr;

            prev = curr;
            prev_tail = getTailLinkedList(prev);
        }


        int * countP = NULL;
        if ((countP = calloc(4, sizeof(int))) == NULL) {
            perror("countP");
            exit(0);
        }

        char * charP = NULL;
        if ((charP = calloc(1, sizeof(char))) == NULL) {
            perror("charP");
            exit(0);
        }

        curr = main_head;
        while (curr != NULL)
        {
            * countP = curr->count;
            * charP  = curr->character;

            fwrite(countP, sizeof(int), 4, stdout);
            fwrite(charP, sizeof(char), 1, stdout);
            curr = curr->next;
        }

        // free all allocated memory
        free(countP);
        free(charP);
        free(thread_list);
        free(head_list);

        freeLinkedList(main_head);
        pthread_mutex_destroy(&head_list_lock);
    }

    else printf("my-zip: file1 [file2 ...]\n");

    return 0;
}