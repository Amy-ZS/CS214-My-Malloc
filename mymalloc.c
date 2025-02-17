#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mymalloc.h"

#define MEMSIZE 4096     // size of memory pool  4096 bytes
#define HEADERSIZE 8      // size of header  8 bytes
#define OBJECTS 64        // number of objects  64          

