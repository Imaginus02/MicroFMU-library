#include "ufmu_alloc.h"
#include "py/obj.h"

// #TODO: Add proper calloc and realloc functions

void* ufmu_malloc(unsigned int number_of_element, unsigned int size_of_an_element) {
    unsigned int total_size = number_of_element * size_of_an_element;
    void* ptr;
    
    #ifdef MICROPY_ESP_IDF_4
        // ESP32 port - direct allocation
        ptr = m_malloc(total_size);
    #else
        // Unix port - allocation with header and storage of size
        ptr = m_malloc(total_size + UFMU_ALLOC_HEADER_SIZE);
        if (ptr != NULL) {
            *((unsigned int*)ptr) = total_size;

            ptr = (char*)ptr + UFMU_ALLOC_HEADER_SIZE;
        }
    #endif
    
    return ptr;
}

void ufmu_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    
    #ifdef MICROPY_ESP_IDF_4
        // ESP32 port - direct free
        m_free(ptr);
    #else
        // Unix port - retrieve size from header
        void* header_ptr = (char*)ptr - UFMU_ALLOC_HEADER_SIZE;
        unsigned int size = *((unsigned int*)header_ptr);
        m_free(header_ptr, size + UFMU_ALLOC_HEADER_SIZE);
    #endif
}