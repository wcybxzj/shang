#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void *my_malloc(size_t size);
void *my_calloc(size_t num, size_t size);
void *my_realloc(void *src_data_ptr, size_t size);
void  my_free(void *ptr);
size_t get_malloc_size(void *data_ptr);
