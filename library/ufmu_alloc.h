#define UFMU_ALLOC_HEADER_SIZE 4

// #TODO: Add proper calloc and realloc functions

void * ufmu_malloc(unsigned int n, unsigned int size);
void ufmu_free(void *ptr);