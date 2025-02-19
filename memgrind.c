#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void stress_test() {
    void *ptrs[120];

    //testA
    for(int i = 0; i < 120; i++) {
        void *ptr = malloc(1);
        free(ptr);
    }

    //testB
    for(int i = 0; i < 120; i++) {
        ptrs[i] = malloc(1);
    }
    for(int i = 0; i < 120; i++) {
        free(ptrs[i]);
    }

    //testC
    void *allocated[120];
    int allocated_count = 0;
    for (int i = 0; i < 120; i++) {
        if(rand() % 2 == 0) {
            ptrs[i] = malloc(1);
            allocated[allocated_count] = ptrs[i];
            allocated_count++;
        } else{
            free(allocated[allocated_count - 1]);
            allocated_count--;
        }
    }
    for (int i = 0; i < allocated_count; i++) {    
        free(allocated[i]);
    }

    //testD
    #define MAX_Size 80
    void *allocated2[120];
    int allocated_count2 = 0;
    for(int i = 0; i < 120; i++) {
        if(rand() % 2 == 0) {
            size_t size = rand() % MAX_Size + 1;
            void *ptr = malloc(size);
            allocated2[allocated_count2] = ptr;
            allocated_count2++;
        } 
        else {
            int index = rand() % allocated_count2;
            free(allocated2[index]);
            allocated2[index] = allocated2[allocated_count2 - 1];
            allocated_count2--;
        }
    }
    for (int i = 0; i < allocated_count2; i++) {
        free(allocated2[i]);
    }

    //testE
    #define MAX_Size2 80
    #define MIN_Size2 1
    void *allocated3[120];
    int allocated_count3 = 0;
    for(int i = 0; i < 120; i++) {
        size_t size = rand() % (MAX_Size2 - MIN_Size2 + 1) + MIN_Size2;
        void *ptr = malloc(size);
        if(ptr == NULL) {
            continue;
        }
        allocated3[allocated_count3] = ptr;
        allocated_count3++;
    }
    for(int i = 0; i < allocated_count3/2; i++) {
        int index = rand() % allocated_count3;
        free(allocated3[index]);
        allocated3[index] = allocated3[allocated_count3 - 1];
        allocated_count3--;
    }
    for(int i = 0; i < allocated_count3/2; i++) {
        size_t size = rand() % (MAX_Size2 - MIN_Size2 + 1) + MIN_Size2;
        void *ptr = malloc(size);
        if(ptr == NULL) {
            continue;
        }
        printf("Success\n");
        free(ptr);
    }
    for(int i = 0; i < allocated_count3; i++) {
        free(allocated3[i]);
    }
}
