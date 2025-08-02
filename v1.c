/* For a data processor, can you use a doubly linked first_blob to store data?

The idea is to represent a “blob” of data through a struct that contains

a string (for the data) and pointers to preceding and following blobs. 


Can you build a function that splits a data blob in two at a given point?
One that joins two consecutive data blobs?
One that runs through the entire data and puts it in the form of one blob per line?
Can you create a function that prints the entire data or prints until the data is cut off due to the screen size? */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOB_SIZE 4

struct blob {
    struct blob* next_blob;
    struct blob* prev_blob;
    char* data;
} first_blob;

/* Data */
char* text = 0;
size_t text_len = 0;
size_t total_blobs = 0;


/* Declarations */
void initBlobData(struct blob* current, size_t i);
void initNextBlob(struct blob*,  size_t idx);
void initPrevBlob(struct blob* current, size_t i, struct blob* prev_blob);
int initText();
void initList();
int initData();
void freeAll(struct blob* current, int idx);

int main(int argc, char const *argv[]) {
    if (initData() == -1) return EXIT_FAILURE;

    printf("%s\n", first_blob.data);
    printf("%s\n", first_blob.next_blob->next_blob->next_blob->next_blob->next_blob->data);

    printf("%s\n", first_blob.next_blob->prev_blob->data);

    
    freeAll(&first_blob, 0);
    return EXIT_SUCCESS;
}

/* Init Struct */
void initNextBlob(struct blob* current, size_t idx) {
    initBlobData(current, idx);
    size_t i = idx;
    if (i == total_blobs - 1) {
        current->next_blob == NULL;
        return;
    } else {
        i++;
        current->next_blob = malloc(sizeof(first_blob));
        initNextBlob(current->next_blob, i);
    } 
    return;   
}

void initPrevBlob(struct blob* current, size_t idx, struct blob* prev_blob) {
    size_t i = idx;
    if (!i) {
        current->prev_blob = NULL;
        i++;
        initPrevBlob(current->next_blob, i, current);
    }
    else if (i != total_blobs) {
        current->prev_blob = prev_blob;
        i++;
        initPrevBlob(current->next_blob, i, current);
    }
    return;
}

/* Init data */
void initBlobData(struct blob* current, size_t i) {
    const char* pointer = text + (i * BLOB_SIZE); // points to position in text that will be copied to ...->data
    current->data = malloc(BLOB_SIZE + 1);
    strncpy(current->data, pointer, BLOB_SIZE);
    // if copied less than BLOB_SIZE then strncpy automtically adds \0's

    current->data[BLOB_SIZE] = '\0';
}

void initList() {
    first_blob.data = malloc(BLOB_SIZE + 1);
    first_blob.next_blob = NULL;
    first_blob.prev_blob = NULL;
}

int initText() { // initialize ONE line from the file
    FILE* fp = fopen("output.txt", "r");
    
    int status = getline(&text, &text_len, fp);
    fclose(fp);
    if (status == -1) return -1;
    
    total_blobs = text_len / BLOB_SIZE;
    return 0;
}

int initData() {
    if (initText() == -1) return EXIT_FAILURE;
    initList();
    if (first_blob.data == NULL) return EXIT_FAILURE;
    initNextBlob(&first_blob, 0);
    initPrevBlob(&first_blob, 0, NULL);
    return EXIT_SUCCESS;
}

/* Free data */
void freeAll(struct blob* current, int idx) {
    int i = idx;
    if (i != total_blobs - 1) {         // -1 because next_blob for the last blob isn't allocated
        free(current->data);
        i++;                            // no need to free prev_blobs becuse they're just pointers
        freeAll(current->next_blob, i); // initNext already allocated memory for a blob
        free(current->next_blob);
    }
}
