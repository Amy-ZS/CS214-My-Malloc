#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include "mymalloc.h"
//#include <my

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

void * mymalloc(size_t size, char *file, int line){
    printf("mymalloc called from file %s at line %d\n", file, line);
    if(!is_initialized){
        myalloc_init();
    }
    // Cast the start of the heap to a header pointer
    header *current = (header*)heap.bytes;
    
    // Find the first block that is large enough
    while(current->size < size || !current->free){
        // If there is no next block, return NULL
        if(current->next == NULL){
            printf("Error: Unable to allocate memory in file %s at line %d\n", file, line);
            return NULL;
        }
        // Move to the next block
        current = current->next;
        printf("#########\n");
    }
    
    // If the block is larger than the requested size plus the size of the header
    if(current->size > size + HEADERSIZE){
        printf("*********\n");
        // Create a new block after the current block
        header *new_block = (header*)((char*)current + HEADERSIZE + size);
        
        // Set the size of the new block to the size of the current block minus the requested size and header size
        new_block->size = current->size - size - HEADERSIZE;
        
        // The new block is free
        new_block->free = 1;
        
        // The new block points to the next block
        new_block->next = current->next;
        
        // The current block is now the requested size
        current->size = size;
        
        // The current block is no longer free
        current->free = 0;
        
        // The current block points to the new block
        current->next = new_block;
    } 
    else if(current->size >= size){
        // The current block is now the requested size
        printf("$$$$$$$$$\n");
        current->free = 0;
        current->next = NULL;
        current->size = size;
    }
    
    // Return a pointer to the start of the block
    printf("Allocated %zu bytes in file %s at line %d\n", size, file, line);
    printf("allocating block at address %p with size %d\n", current, current->size);
    return (char*)current + HEADERSIZE;
}

void myfree(void *ptr, char *file, int line){
    printf("myfree called from file %s at line %d\n", file, line);
    if (!is_initialized){
        myalloc_init();
    }

    // Cast the pointer to a header pointer
    header *current = (header*)((char*)ptr - HEADERSIZE);
    printf("Freeing block at address %p with size %d\n", current, current->size);
    // Mark the block as free
    current->free = 1;
    
    // If the next block is free, merge the two blocks
    //printf("%p, %d\n", current->next, current->next->free);
    printf("^^^^^^^^^^^^\n");
    
    
    printf("Freed memory in file %s at line %d\n", file, line);
}