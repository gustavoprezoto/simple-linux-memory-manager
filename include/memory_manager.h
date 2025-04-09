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
 * @def ITERATE_PAGE_FAMILIES_BEGIN
 * @brief Begins a loop over all valid vm_page_family_t entries within a vm_page_for_families_t.
 *
 * This macro iterates through the array of struct families stored in a page.
 * It stops if it encounters a family with struct_size == 0 or reaches the
 * maximum number of families allowed in a single page.
 *
 * Must be paired with @ref ITERATE_PAGE_FAMILIES_END.
 *
 * @param vm_page_for_families_ptr Pointer to the vm_page_for_families_t to iterate over.
 * @param curr A vm_page_family_t* variable used as the loop iterator.
 */
#define ITERATE_PAGE_FAMILIES_BEGIN(vm_page_for_families_ptr, curr)         \
    {                                                                       \
        int __count = 0;                                                    \
        for (;                                                              \
             __count < MM_MAX_FAMILIES_PER_VM_PAGE;                       \
             __count++)                                                     \
        {                                                                   \
            curr = &vm_page_for_families_ptr->vm_page_family[__count];

/**
 * @def ITERATE_PAGE_FAMILIES_END
 * @brief Ends the loop started by @ref ITERATE_PAGE_FAMILIES_BEGIN.
 *
 * Closes the loop block for iterating over struct families in a page.
 *
 * @param vm_page_for_families_ptr Same pointer passed to the BEGIN macro.
 * @param curr The iterator variable used in the BEGIN macro.
 */
#define ITERATE_PAGE_FAMILIES_END() \
    }                                                             \
    }

/**
 * @def MM_MAX_FAMILIES_PER_VM_PAGE
 * @brief Calculates the maximum number of vm_page_family_t entries that can fit in a single memory page.
 *
 * Each vm_page_for_families structure contains a pointer to the next page and a flexible array of
 * vm_page_family_t entries. This macro subtracts the size of the `next` pointer from the system page size
 * and divides the remaining space by the size of each family entry to determine how many families
 * can fit in one page.
 *
 * This value is used during memory manager initialization to allocate and organize struct families efficiently.
 *
 * @note Assumes that each family page is dedicated entirely to holding vm_page_family_t entries.
 */
#define MM_MAX_FAMILIES_PER_VM_PAGE \
    (SYSTEM_PAGE_SIZE - sizeof(vm_page_for_families_t *)) /\
        sizeof(vm_page_family_t)

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
