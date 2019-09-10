#include "mem.h"
#include <malloc.h>

void  *mem_malloc(unsigned int size)
{
    return malloc(size);
}
void mem_free(void *ptr)
{
    free(ptr);
}


