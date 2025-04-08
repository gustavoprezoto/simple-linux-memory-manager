#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <sys/mman.h>  // For mmap(), munmap(), etc.

#ifndef MAP_ANON
#define MAP_ANON MAP_ANONYMOUS  // Compatibility for systems that don't define MAP_ANON
#endif

#define MM_MAX_STRUCT_NAME 64  // Maximum allowed length for a struct family name

#define TRUE 1
#define FALSE 0

/**
 * @brief Represents a struct "family" definition.
 *
 * Each family corresponds to a specific struct type,
 * identified by a name and a fixed size in bytes.
 */
typedef struct vm_page_family {
    char struct_name[MM_MAX_STRUCT_NAME];  /**< Name of the struct (e.g., "socket", "hash_node") */
    __uint32_t struct_size;                /**< Size of the struct in bytes */
} vm_page_family_t;

/**
 * @brief Represents a virtual memory page that holds multiple
 * struct families.
 *
 * It contains a pointer to the next page,
 * and a flexible array of family definitions.
 */
typedef struct vm_page_for_families {
    struct vm_page_for_families *next;     /**< Pointer to the next page of families */
    vm_page_family_t vm_page_family[0];    /**< Flexible array of struct families */
} vm_page_for_families_t;

/**
 * @brief Initializes the memory manager.
 *
 * Must be called before any memory operations.
 */
void mm_init();

/**
 * @brief Instantiates a new struct page family and registers it with the page family manager.
 *
 * This function creates a new vm_page_family_t entry internally and adds it to the global
 * page family registry. Page families help optimize memory management for commonly used
 * struct types by grouping them under dedicated allocation metadata.
 *
 * @param struct_name String containing the name of the struct.
 * 
 * @param struct_size A 32-bit unsigned integer representing the size of the struct in bytes.
 */
void mm_instantiate_new_page_family(char *struct_name, __uint32_t struct_size);


#endif // MEMORY_MANAGER_H
