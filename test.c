#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include "mymalloc.h"

#define MEMSIZE 4096     // size of memory pool  4096 bytes
#define HEADERSIZE 8      // size of header  8 bytes

static union{
    char bytes[MEMSIZE];
    double not_used; // force alignment to double
} heap;

typedef struct header {
    int size;
    struct header *next;
    int free;
} header;

static int is_initialized = 0; // Flag to track initialization

// Function to initialize memory
void myalloc_init() {
    printf("Initializing memory...\n");
    // Cast the start of the heap to a header pointer
    header *first = (header*)heap.bytes;
    
    // Set the size of the first block to the total memory size minus the header size
    first->size = MEMSIZE - HEADERSIZE;
    
    // There is no next block, so set next to NULL
    first->next = NULL;
    
    // Mark the block as free
    first->free = 1;
    
    // Set the initialized flag
    is_initialized = 1;
    printf("Memory initialized.\n");
}

// Function to allocate memory
void * mymalloc(size_t size, char *file, int line){
    if (!is_initialized) {
        myalloc_init();
    }
    // Cast the start of the heap to a header pointer
    header *current = (header*)heap.bytes;
    
    //find the first block that is large enough
    while (current->size < size || current->free == 0) {
        // If there is no next block, return NULL
        if (current == NULL) {
            printf("Error: Unable to allocate memory in file %s at line %d\n", file, line);
            return NULL;
        }
        // Move to the next block
        current = current->next;
    }

    // If the block is larger than the requested size, split it
    if(current -> size > size + HEADERSIZE){
        // Create a new block after the current block
        header *new_block = (header*)((char*)current + HEADERSIZE + size);
        
        // Set the size of the new block to the size of the current block minus the requested size and header size
        new_block->size = current->size - size - HEADERSIZE;
        
        // Set the next block of the new block to the next block of the current block
        new_block->next = current->next;
        
        // Mark the new block as free
        new_block->free = 1;
        
        // Set the size of the current block to the requested size
        current->size = size;
        
        // Set the next block of the current block to the new block
        current->next = new_block;

        // Mark the current block as not free
        current->free = 0;
    }

    //if the block is exactly the size of the requested size, mark it as not free
    else{
        current->free = 0;
        current->next = NULL;
        current->size = size;
    }
    return (char*)current + HEADERSIZE;
}

// Function to free memory
void myfree(void *ptr, char *file, int line){
    printf("Freeing memory...\n");
    // Cast the pointer to a header pointer
    header *current = (header*)ptr;
    
    // Mark the block as free
    current->free = 1;

    // If the next block is free, merge the blocks
    header *head = (header*)heap.bytes;
    while(head->next != NULL){
        // Set the size of the current block to the size of the current block plus the size of the next block plus the header size
        if(head->free == 1 && head->next->free == 1){
            head->size = head->size + head->next->size + HEADERSIZE;
            head->next = head->next->next;
        }
        head = head->next;
    }
}