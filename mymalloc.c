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

void myalloc_init() {
    // initialize memory pool
    *(int *)heap = MEMSIZE - HEADERSIZE;
    *(int *)(heap + 4) = 0;
}

void myalloc(size_t size, char *file, int line) {
    int i, *p, *q;
    // find first object that fits
    for (i = 0; i < OBJECTS; i++) {
        if (obj[i] == NULL) {
            if (size + HEADERSIZE <= *(int *)heap) {
                obj[i] = heap + HEADERSIZE;
                p = (int *)obj[i] - 1;
                *p = size;
                q = (int *)heap;
                *q -= size + HEADERSIZE;
                *(int *)(obj[i] + size) = size;
                *(int *)(obj[i] + size + 4) = 0;
                return;
            }
        }
    }
    fprintf(stderr, "Out of memory in %s at line %d\n", file, line);
    exit(1);
}

void myfree(char *p, char *file, int line) {
    int i, *q;
    // find object
    for (i = 0; i < OBJECTS; i++) {
        if (obj[i] == p) {
            q = (int *)p - 1;
            *(int *)(p + *q) = *(int *)heap;
            *(int *)(p + *q + 4) = 0;
            *(int *)heap += *q + HEADERSIZE;
            obj[i] = NULL;
            return;
        }
    }
    fprintf(stderr, "Attempt to free unallocated memory in %s at line %d\n", file, line);
    exit(1);
}