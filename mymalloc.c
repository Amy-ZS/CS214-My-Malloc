#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mymalloc.h"

#define MEMSIZE 4096     // size of memory pool  4096 bytes
#define HEADERSIZE 8      // size of header  8 bytes

static union{
    char bytes[MEMSIZE];
    double not_used; // force alignment to double
} heap;

typedef struct header {
    int size;
    header *next;
    int free;
} header;

void myalloc_init(){
    // Cast the start of the heap to a header pointer
    header *first = (header*)heap.bytes;
    
    // Set the size of the first block to the total memory size minus the header size
    first->size = MEMSIZE - HEADERSIZE;
    
    // There is no next block, so set next to NULL
    first->next = NULL;
    
    // Mark the block as free
    first->free = 1;
}

void * mymalloc(size_t size, char *file, int line){
    if(! myalloc_init){
        myalloc_init();
    }
    // Cast the start of the heap to a header pointer
    header *current = (header*)heap.bytes;
    
    // Find the first block that is large enough
    while(current->size < size || !current->free){
        // If there is no next block, return NULL
        if(current->next == NULL){
            return NULL;
        }
        // Move to the next block
        current = current->next;
    }
    
    // If the block is larger than the requested size plus the size of the header
    if(current->size > size + HEADERSIZE){
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
    } else {
        // The current block is now the requested size
        current->free = 0;
    }
    
    // Return a pointer to the start of the block
    return (char*)current + HEADERSIZE;
}

void myfree(void *ptr, char *file, int line){
    if (! myalloc_init){
        myalloc_init();
    }

    // Cast the pointer to a header pointer
    header *current = (header*)((char*)ptr - HEADERSIZE);
    
    // Mark the block as free
    current->free = 1;
    
    // If the next block is free, merge the two blocks
    while(current->next != NULL && current->next->free){
        // Add the size of the next block to the current block
        current->size += HEADERSIZE + current->next->size;
        
        // The current block now points to the block after the next block
        current->next = current->next->next;
    }
}
