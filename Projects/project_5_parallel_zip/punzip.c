#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <time.h>


// SOURCES
// https://linux.die.net/
// https://man7.org/linux/man-pages/
// for libc funcs
//
// https://www.learn-c.org/en/Linked_lists
// for linked lists
//
// https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html
//

// "head" = head of the linked list, aka the first node in linked list



// ======== STRUCTS ========
// Linked list node structure
typedef struct ll_node {
    // Data is either struct num_char or th_data_struct
    void * data;
    struct ll_node * next;
} ll_node_t;


// Datatype for num char pairs created by compression
// ... 10A, B36, K3, Y12, ...
typedef struct num_char {
    char character;
    int count;
} num_char_t;


// Datatype threads access to get next task
// filename = what file the need to access and process
// index at what index in head_list should the created num_char list go
typedef struct th_data_struct {
    char * filename;
    int    index;
} th_data;




// ======== GLOBALS FOR THREADS ========
// List of list heads
// list heads contain the head of every llist a thread makes
// every thread has their own index to store their list head in head_list
ll_node_t ** head_list = NULL;
// Global head_list_lock
pthread_mutex_t head_list_lock;

// Number that tells how many threads are busy
int busy_th_count = 0;
// Global lock for busy_threads
pthread_mutex_t busy_th_count_lock;





// ======== LINKED LIST FUNCTIONS ========
// To init list and return head
ll_node_t * initList(void * data) {

    ll_node_t * head;
    if ((head = (ll_node_t *) calloc(1, sizeof(ll_node_t))) == NULL) {
        perror("init linked");
        exit(-1);
    }
    head->data      = data;
    head->next      = NULL;

    return head;
}

// Function to add node to the end of list
int addToEndOfList(ll_node_t * head, void * data) {
    ll_node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    // Variables
    if ((current->next = (ll_node_t *) calloc(1, sizeof(ll_node_t))) == NULL) {
        perror("add to linked");
        exit(-1);
    }
    current->next->data         = data;
    current->next->next         = NULL;
    return 0;
}

// Function to traverse and free the list
int freeLinkedList(ll_node_t * head) {
    if (head != NULL) {

        ll_node_t * temp1 = head;
        ll_node_t * temp2 = NULL;

        while ((temp2 = temp1->next) != NULL) {
            free(temp1->data);
            free(temp1);
            temp1 = temp2;
        }
        free(temp1->data);
        free(temp1);
        free(temp2);
    }
    return 0;
}

// Function to return pointer to the tail of given list
ll_node_t * getTailLinkedList(ll_node_t * head) {
    if (head != NULL) {

        ll_node_t * curr = head;
        // Traverse to the end of list
        while (curr->next != NULL)
        {
            curr = curr->next;
        }
        return curr;
    }
    return NULL;
}

// pop function removes and returns the head of the list
// while updating the head to be the next in list
ll_node_t * popLinkedList(ll_node_t ** head) {
    if (* head != NULL) {
        ll_node_t * first =  * head;
        ll_node_t * next  = (* head)->next;
        * head = next; // replacing head with next
        return first;
    }
    return NULL;
}







// The function the threads will be running
// void * in_data is of type th_data
void * threadFunction(void * in_data) {

    th_data * data  = (th_data *) in_data;
    char * filename = data->filename;
    int file_num    = data->index;

    FILE * file;

    int * intP;
    char * charP;

    // this thread's local linked list head
    ll_node_t * head = NULL;

    // get lock before using global int
    pthread_mutex_lock(&busy_th_count_lock);
    // add one being busy (this thread)
    busy_th_count++;
    pthread_mutex_unlock(&busy_th_count_lock);


    // intializing pointers
    if ((intP = (int *) calloc(4, sizeof(int))) == NULL) {
        perror("intP");
        exit(-1);
    }

    if ((charP = (char *) calloc(1, sizeof(char))) == NULL) {
        perror("charP");
        exit(-1);
    }


    // opening given file
    if ((file = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "error: cannot open file '%s'\n", filename);
        exit(1);
    }

    while (1) {

        // Iterate through file
        // intP = how many of letter charP
        if (fread(intP, sizeof(int), 4, file) != 4) {
            break; // EOF
        }

        if (fread(charP, sizeof(char), 1, file) != 1) {
            break; // EOF
        }

        // allocate memory for one num_char pair (10A, 34B, 4K, ...)
        num_char_t * num_char_pair;
        if ((num_char_pair = calloc(1, sizeof(num_char_t))) == NULL) {
            perror("num_char_pair");
            exit(-1);
        }

        num_char_pair->character = * charP;
        num_char_pair->count     = * intP;

        // Save to this thread's linked list
        if (head != NULL) {
            addToEndOfList(head, num_char_pair);
        } else {
            // first time run needs to initialize the list
            head = initList(num_char_pair);
        }
    }



    // get lock before using global list
    pthread_mutex_lock(&head_list_lock);
    // add the list head of this thread to global head_list
    head_list[file_num] = head;
    pthread_mutex_unlock(&head_list_lock);


    // get lock before using global int
    pthread_mutex_lock(&busy_th_count_lock);
    // remove one busy thread (this)
    busy_th_count--;
    pthread_mutex_unlock(&busy_th_count_lock);


    // free and detach all thread memory
    free(intP);
    free(charP);
    fclose(file);
    free(data);
    pthread_detach(pthread_self());
    
    return NULL;
}





int main(int argc, char * argv[]) {

    ll_node_t * main_head = NULL;

    // Number of system available threads
    int th_count = get_nprocs();

    // Number of files to be read
    int file_count = argc-1;
    
    // No need to create more threads than there are files to assign them
    if (file_count < th_count) {
        th_count = file_count;
    }

    // files to be unzipped head
    ll_node_t * file_list_head = NULL;



    if (file_count >= 1) { // One or more files are given


        // List of pointers to list heads created for each file
        if ((head_list = calloc(file_count, sizeof(ll_node_t *))) == NULL) {
            perror("head_list");
            exit(-1);
        }
        // Initialize lock file list heads
        if (pthread_mutex_init(&head_list_lock, NULL) != 0) {
            perror("head list lock init");
            exit(-1);
        }


        // Initialize lock for busy thread count
        if (pthread_mutex_init(&busy_th_count_lock, NULL) != 0) {
            perror("stat list lock init");
            exit(-1);
        }


        // add files to list from argv
        // every available thread is assigned next in this linked list
        // to be processed and zipped
        for (size_t idx = 0; idx < file_count; idx++) {
            th_data * data;
            if ((data = calloc(1, sizeof(th_data))) == NULL) {
                perror("add files to list");
                exit(-1);
            }
            data->index = idx;
            data->filename = argv[idx +1];

            // Init if doesn't exist
            if (file_list_head == NULL) {
                file_list_head = initList(data);

            } else {
                addToEndOfList(file_list_head, data);
            }
        }
        

        // As long as there are un-processed files do the while loop
        ll_node_t * next_file = popLinkedList(&file_list_head);
        int busy_cnt;
        struct timespec loop_sleep, rem_sleep;
        loop_sleep.tv_sec  = 0;
        loop_sleep.tv_nsec = 100;

        while (next_file != NULL)
        {
            pthread_mutex_lock(&busy_th_count_lock);
            busy_cnt = busy_th_count;
            pthread_mutex_unlock(&busy_th_count_lock);

            // if there are free threads give them jobs
            if (th_count - busy_cnt > 0) {

                th_data * data = (th_data *) next_file->data;

                // create a new thread and give it the th_data data
                pthread_t thread_id;
                if (pthread_create(&thread_id, NULL, threadFunction, (void *) data) != 0) {
                    perror("thread create");
                    exit(-1);
                }
                free(next_file);
                // get next from list
                next_file = popLinkedList(&file_list_head);
            }
            
            // sleep for 100ns = 0.1ms
            // sleep to not occupy the lock
            if (nanosleep(&loop_sleep, &rem_sleep) < 0) {
                perror("nanosleep failed");
                exit(-1);
            }
        }
        
        
        // crude but a working way to wait for all threads to finish
        while (1)
        {
            pthread_mutex_lock(&busy_th_count_lock);
            if (busy_th_count < 1) break;
            pthread_mutex_unlock(&busy_th_count_lock);


            // sleep for 100ns = 0.1ms
            // sleep to not occupy the lock
            if (nanosleep(&loop_sleep, &rem_sleep) < 0) {
                perror("nanosleep failed");
                exit(-1);
            }
        }
        


        // Creating a chain of linked lists
        // 0 -> 1 -> 2 -> ...
        // get the first head as the main head
        main_head = head_list[0];
        if (main_head == NULL) {
            fprintf(stderr, "list chain is null\n");
            exit(-1);
        }
        ll_node_t * prev = main_head;
        ll_node_t * prev_tail = getTailLinkedList(prev);
        ll_node_t * curr;

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

        // Decompress the main list iteratively
        curr = main_head;
        while (curr != NULL)
        {

            num_char_t * num_char_pair;
            num_char_pair = (num_char_t *) curr->data;


            * countP = num_char_pair->count;
            * charP  = num_char_pair->character;
            
            // One way to decompress A3 = AAA etc.
            for (size_t n = 0; n < * countP; n++)
                printf("%c", *charP);
            
            curr = curr->next;
        }

        // free all allocated memory
        free(countP);
        free(charP);
        free(head_list);

        freeLinkedList(file_list_head);
        freeLinkedList(main_head);
        pthread_mutex_destroy(&head_list_lock);
    }

    else printf("punzip: file1 [file2 ...]\n");

    return 0;
}