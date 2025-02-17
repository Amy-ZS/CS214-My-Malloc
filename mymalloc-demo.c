#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mymalloc.h"


static char memory[MMEMSIZE];
static meta_t *free_list = NULL;
static int heap_initizlied = 0;

void mymalloc_initilized(){

}