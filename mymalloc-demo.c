#include <stddef.h>
#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//#include <errno.h>
//extern int errno; errno = ENOMEM;

#define MEMLENGTH 4096
#define MAGIC 0xDEADBEEF


typedef struct memory_block{
    size_t size;          
    int is_free;             
    struct memory_block *next;
    unsigned int magic_number;
    }memory_block_t;

    static union {
        char bytes[MEMLENGTH];
        double not_used;
    }heap;

static memory_block_t *free_list = NULL;
static int pool_initialized = 0;


static void initialize_free_list(void); 
static void initialize_pool(void);
static void leak_detector(void); 
static void split_block(memory_block_t *block, size_t size);
static void coalesce(memory_block_t *block); 
size_t align8(size_t size);
memory_block_t *find_free_block(size_t size);

void *mymalloc(size_t size, char *file, int line) {
    if (!pool_initialized) {
        initialize_pool();
    }

    size = align8(size);

    memory_block_t *block = find_free_block(size);
    errno = ENOMEM;
    if (block == NULL) {
        fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
        return NULL;
    }
    split_block(block, size);

    block->is_free = 0;
    block->magic_number = MAGIC;
    return (void *)(block + 1);
}

void myfree(void *ptr, char *file, int line) {
    errno = ENOMEM;
    if (ptr == NULL) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        return;
    }

    memory_block_t *block = (memory_block_t *)ptr - 1;

    if ((char*)block < heap.bytes || (char*)block >= heap.bytes + MEMLENGTH) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    if (block->magic_number != MAGIC) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    if (block->is_free) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    block->is_free = 1;

    coalesce(block);
    leak_detector();
}

static void initialize_pool(void) {
    if(!pool_initialized){
    errno = ENOMEM;
    initialize_free_list();
    pool_initialized = 1;
    atexit(leak_detector);}
}

static void initialize_free_list(void) {
    free_list = (memory_block_t *)heap.bytes;
    errno = ENOMEM;
    if (free_list == NULL) {
        fprintf(stderr, "initialize_free_list: Failed to initialize free list.\n");
        return;
    }
    free_list->size = MEMLENGTH;
    free_list->is_free = 1;
    free_list->next = NULL;
    free_list->magic_number = MAGIC;
}

memory_block_t *find_free_block(size_t size) {
    errno = ENOMEM;
    if (free_list == NULL) {
        fprintf(stderr, "initialize_free_list: Failed to initialize free list.\n");
        return NULL;
    }
    memory_block_t *current = free_list;

    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

static void split_block(memory_block_t *block, size_t size) {
    errno = ENOMEM;
    if (block == NULL) {
        fprintf(stderr, "split_block: NULL block pointer.\n");
        return;
    }
    if (block->size >= size + sizeof(memory_block_t)) {
        memory_block_t *new_block = (memory_block_t *)((char *)block + size + sizeof(memory_block_t));
        new_block->size = block->size - size - sizeof(memory_block_t);
        new_block->is_free = 1;
        new_block->next = block->next;
        new_block->magic_number = MAGIC;

        block->size = size;
        block->next = new_block;
    }
}

static void coalesce(memory_block_t *block) {
    errno = ENOMEM;
    if (block == NULL) {
        fprintf(stderr, "coalesce: NULL block pointer.\n");
        return;
    }
    if (block->next && block->next->is_free) {
        //printf("Coalescing block at %p with next block at %p\n", (void*)block, (void*)block->next);
        //printf("Current block size: %zu, Next block size: %zu\n", block->size, block->next->size);

        block->size = block->size + block->next->size + sizeof(memory_block_t);
        block->next = block->next->next;
        //printf("After coalescing, current block size: %zu\n", block->size);}
    }
        memory_block_t *prev = free_list;
        while (prev && prev->next != block){
            prev = prev->next;}
        if(prev&& prev->is_free){
            //printf("Coalescing previous block at %p with current block at %p\n", (void*)prev, (void*)block);
            //printf("Previous block size: %zu, Current block size: %zu\n", prev->size, block->size);

            prev->size += block->size + sizeof(memory_block_t);
            prev->next = block->next;
            //printf("After coalescing, previous block size: %zu\n", prev->size);
        }
    }

static void leak_detector(void){
    memory_block_t *current = free_list;
    size_t total_leaked = 0;
    size_t total_count = 0;
    while(current != NULL){
        if(!current->is_free){
        total_leaked += current->size;
        total_count++;}
        current = current->next;}
        if(total_count > 0){
            fprintf(stderr, "mymalloc: %zu bytes leaked in %zu objects.\n", total_leaked, total_count);
        }
    }

size_t align8(size_t size){
    errno = ENOMEM;
        return (size + 7) & ~7;
    }
