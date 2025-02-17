#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mymalloc.h"

#define MEMSIZE 4096     // size of memory pool  4096 bytes
#define HEADERSIZE 8      // size of header  8 bytes
#define OBJECTS 64        // number of objects  64          

char *obj[OBJECTS];     // array of pointers to objects
char heap[MEMSIZE];     // memory pool

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mymalloc.h"

#define MEMSIZE 4096     // size of memory pool  4096 bytes
#define HEADERSIZE 8      // size of header  8 bytes

char heap[MEMSIZE];     // memory pool

typedef struct header {
    int size;
    header *next;
    int free;
} header;

void mymalloc_intialize() {
    // Initialize the head pointer to the start of the heap
    header *head = (header *)heap;
    
    // Set the restsize to the total memory size minus the header size
    head->size = MEMSIZE - HEADERSIZE;
    
    // There is no next block, so set next to NULL
    head->next = NULL;
    
    // Mark the block as free
    head->free = 1;
}

void myalloc(int size, char data) {
    // Initialize the head pointer to the start of the heap
    header *head = (header *)heap;
    
    // Find the first free block that is large enough to hold the data
    while (head->size < size || head->free == 0) {
        head = head->next;
    }
    
    // If the block is larger than the requested size, split the block
    if (head->size > size + HEADERSIZE) {
        header *new = (header *)((char *)head + HEADERSIZE + size);
        new->size = head->size - size - HEADERSIZE;
        new->next = head->next;
        new->free = 1;
        head->next = new;
        head->size = size;
    }
    
    // Mark the block as used
    head->free = 0;
    
    // Copy the data into the block
    memcpy((char *)head + HEADERSIZE, &data, size);
}
