/* For a data processor, can you use a doubly linked list to store data?

The idea is to represent a “blob” of data through a struct that contains

a string (for the data) and pointers to preceding and following blobs. 


~Can you build a function that splits a data blob in two at a given point?~

~One that joins two consecutive data blobs?~

~One that runs through the entire data and puts it in the form of one blob per line?~

~Can you create a function that prints the entire data or prints until the data is cut off due to the screen size?~
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define BLOB_SIZE 8

struct blob {
    struct blob* next_blob;
    struct blob* prev_blob;
    char* data;
} list; // means first blob

/* Data */
size_t total_blobs = 0;

/* Declarations */
void initNextBlob(struct blob* current, int i);
void initPrevBlob(struct blob* current);
struct blob* initBlob(struct blob* current, char* data, int i);

int initText(char* filename, int blob_size);
void freeAll();

void blobSplit(int at, int n);
void blobJoin(int n); // joins two consecutive blobs 
void printAll();

/* Fun */
int main(int argc, char const *argv[]) {
    /* data_mode: -2 means use BLOB_SIZE
       anything else means 1 line per blob */
    
    int data_mode = -2; // allow these to be set by the user?
    char* filename = "split.txt";

    initText(filename, data_mode);

    //printAll();

    freeAll();
    return EXIT_SUCCESS;
}

/* Init Struct */
void initNextBlob(struct blob* current, int i) {
    if (i == -1)
        current->next_blob == NULL;
    else {
        current->next_blob = malloc(sizeof(list));
        //current->data = 0;
        //current->next_blob = NULL;
        //current->prev_blob = NULL;
    }
}

void initPrevBlob(struct blob* current) {
    if (current->next_blob != NULL)
        current->next_blob->prev_blob = current;
}

struct blob* initBlob(struct blob* current, char* data, int i) {
    // data is a line from the file, and contains the newline character
    // receives data, current struct blob* and mode i
    // i == -2 means use BLOB_SIZE
    // anything else uses 1 line for a blob
    // calls init next/prev for current
    // puts data into current
    // returns pointer to the latest blob after initializing the one after that to an empty one

    if (i == -2) {
        size_t len = 0;
        do {
            current->data = malloc(BLOB_SIZE + 1);
            strncpy(current->data, &data[len], BLOB_SIZE);
            current->data[BLOB_SIZE] = '\0';
            len += BLOB_SIZE;
            total_blobs++;
            if (len >= strlen(data)) i = -1;
            initNextBlob(current, i);
            initPrevBlob(current);
            if (current->next_blob != NULL)
                current = current->next_blob;
            else
                break;
        } while (1);
        initNextBlob(current, 0); // a buffer blob to continue if new data comes. needs to be destroyed at eof
        // ^ after do/while above i == -1, so i for in param2 result is undefined 

    } else {
        size_t data_len = strlen(data);
        current->data = malloc(data_len + 1);
        strcpy(current->data, data);
        current->data[data_len] = '\0';
        initNextBlob(current, 0);
        initNextBlob(current->next_blob, 0); // a buffer blob
        initPrevBlob(current);
        total_blobs++;
    }
    return current;
}

/* Init data */
int initText(char* filename, int blob_status) { 
    // blob_status == -2 uses BLOB_SIZE
    // anything else uses 1 line for a blob
    // atexit(freeAll);
    FILE* fp = fopen(filename, "r");
    list.data = 0;
    list.next_blob = NULL;
    list.prev_blob = NULL;
    
    size_t line_len, total_len;
    int status;
    char* data = malloc(1);
    struct blob* current = &list;

    while (line_len = getline(&data, &line_len, fp) != -1) {
        while (line_len > 0 && (data[line_len - 1] == '\n' || data[line_len - 1] == '\r'))
            line_len--;

        current = initBlob(current, data, blob_status);
        current = current->next_blob;
    }
    current = initBlob(current, data, blob_status);
    current = current->next_blob;
    fclose(fp);
    free(data);

    return 0;
}

/* Free data */
void freeAll() {
    struct blob* current = &list;
    while (current->next_blob != NULL)
        current = current->next_blob;
    while (current->prev_blob != NULL){
        free(current->data);
        current = current->prev_blob;
        free(current->next_blob);
    }
    free(current->data);
    free(current->next_blob);
}

/* Operations */
void blobSplit(int at, int n) {
    /* "list" is blob for n == 1 */

    at = at <= 0 ? 1 : at;
    n = n > total_blobs ? total_blobs : n;

    struct blob* new;
    struct blob* current = &list;
    while (n > 1) {
        current = current->next_blob;
        n--;
    }

    size_t orig_len = strlen(current->data);
    at = at >= orig_len ? orig_len - 1 : at;
    if (at <= 0) return;

    new = malloc(sizeof(list));
    new->next_blob = current->next_blob;
    if (current->next_blob->prev_blob != NULL)
        current->next_blob->prev_blob = new;

    char* after = &current->data[at]; // buffer
    size_t len = strlen(after);

    new->data = malloc(len + 1);
    strcpy(new->data, after);
    new->data[len] = '\0';

    current->data = realloc(current->data, orig_len - len + 1);
    current->data[orig_len - len] = '\0';
    
    new->prev_blob = current;
    current->next_blob = new;
    total_blobs++;
}

void blobJoin(int n) {
    /* joins two consecutive blobs
       realloc memory for current->data
       append current->data with data of the next blob
       change ptrs of current->next->next->prev, current->next
       and destroy (n+1)th blob */

    if (n >= total_blobs) return;

    struct blob* current = &list;
    while (n > 1) {
        current = current->next_blob;
        n--;
    }

    size_t new_len = strlen(current->data) + strlen(current->next_blob->data);
    current->data = realloc(current->data, new_len + 1);

    strcat(current->data, current->next_blob->data);
    free(current->next_blob->data);

    current->next_blob = current->next_blob->next_blob;
    free(current->next_blob->prev_blob); // ?!
    current->next_blob->prev_blob = current;

    total_blobs--;
}

void printAll() {
    struct blob* current = &list;
    while (current->next_blob->data != NULL) {
        printf("%s", current->data);
        current = current->next_blob;
    }
}
