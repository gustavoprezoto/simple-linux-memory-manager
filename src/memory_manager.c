#include "../include/memory_manager.h"
#include <stdio.h>
#include <memory.h>
#include <unistd.h>

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

static vm_page_for_families_t *first_family_group_page = NULL;
static vm_page_for_families_t *last_family_group_page = NULL;
static size_t SYSTEM_PAGE_SIZE = 0;

void mm_init() {
    SYSTEM_PAGE_SIZE = getpagesize();
}

/**
 * @brief Allocates virtual memory pages directly in kernel.
 *
 * This function uses POSIX mmap syscall to map a new region in private and 
 * anonymous memory with read, execution(!) and write permissions.
 *
 * @param units Number of pages to be allocked.
 *              Each unit corresponds to SYSTEM_PAGE_SIZE bytes.
 *
 * @warning This function uses PROT_EXEC flag. Use it carefully.
 *
 * @return A pointer to an allocked memory in case of success.
 *         Returns NULL in case of error.
 */
static void* __mm_get_new_vm_pages_from_kernel(int units) {
    const size_t map_size = units * SYSTEM_PAGE_SIZE;
    
    char *mem = mmap(
        0,
        map_size,
        PROT_READ | PROT_WRITE | PROT_EXEC,
        MAP_ANON | MAP_PRIVATE,
        0, 0);

    if (mem == MAP_FAILED) {
        printf("Error: Map failed.");
        return NULL;
    }

    memset(mem, 0, map_size);
    
    return mem;
}

/**
 * @brief Deallocates virtual memory pages directly in kernel.
 *
 * This function uses POSIX munmap syscall to deallock a previously mapped memory page.
 *
 * @param *page Pointer to the page that is allocked.
 * 
 * @param units Number of pages to be deallocked.
 *              Each unit corresponds to SYSTEM_PAGE_SIZE bytes.
 */
static void __mm_release_vm_pages_to_kernel(void *page, int units) {
    if (munmap(page, units * SYSTEM_PAGE_SIZE)) {
        printf("Error: Failed using munmap syscall to unmap page.\n");
    }
}

/**
 * @brief This internal function is used for creating a new vm_page_for_families_t 
 * with a first initialized page family inside.
 * 
 * This function is used internally to create the first global page for families.
 * 
 * @param *struct_name Struct name string.
 * 
 * @param struct_size An uint32 containing the size of the struct.
 * 
 * @return A pointer to a first initialized vm_page_for_families_t object.
 */
static vm_page_for_families_t* __mm_create_new_metadata_page_for_struct_families(char *struct_name, __uint32_t struct_size) {
    vm_page_for_families_t *page = (vm_page_for_families_t*) __mm_get_new_vm_pages_from_kernel(1);

    // Creates the first page family entry
    strncpy(page->vm_page_family[0].struct_name, struct_name, sizeof(page->vm_page_family[0].struct_name));
    page->vm_page_family[0].struct_size = struct_size;

    return page;
}

/**
 * @brief This internal function is used for adding a new struct to a metadata
 * family page. 
 * 
 * Iterates to find the next available family metadata group to add the new struct.
 * It creates a new family metadata page if all pages are fully occupied.
 * 
 */
static void __mm_add_struct_to_global_families(char *struct_name, __uint32_t struct_size) {    
    // Iterates to find the next available family metadata group to add the new struct.
    // It creates a new family metadata page if all pages are occupied.
    vm_page_family_t *page_family_iter = NULL;
    vm_page_for_families_t *metadata_page_iter = first_family_group_page;
    while (TRUE) {
        // Iterates the metadata_page current to find if there is a available spot in it.
        ITERATE_PAGE_FAMILIES_BEGIN(metadata_page_iter, page_family_iter) {
            if (page_family_iter->struct_size == NULL || page_family_iter->struct_size == 0) {
                strncpy(page_family_iter->struct_name, struct_name, sizeof(page_family_iter->struct_name));
                page_family_iter->struct_size = struct_size;
                return;
            }
        } ITERATE_PAGE_FAMILIES_END()

        // If it checked all metadata pages and there is no empty spot for it, 
        // then it creates a new metadata page.
        if (metadata_page_iter->next == NULL) {
            vm_page_for_families_t *new_family_metadata_page = __mm_create_new_metadata_page_for_struct_families(struct_name, struct_size);
            new_family_metadata_page->next = first_family_group_page;
            first_family_group_page = new_family_metadata_page;
            return;
        }
        // If there is still pages to be iterated, switch iter to it.
        else {
            metadata_page_iter = first_family_group_page->next;
        }
    }

}

void mm_instantiate_new_page_family(char *struct_name, __uint32_t struct_size) {
    if (struct_size > SYSTEM_PAGE_SIZE) {
        printf("Error: Struct (%s) size is bigger than accepted by an OS page. Skipping.\n", struct_name);
        return;
    }

    // If the first family metadata page doesnt exists.
    if (!first_family_group_page) {
        first_family_group_page = __mm_create_new_metadata_page_for_struct_families(struct_name, struct_size);
        return;
    }

    // Iterates through metadata pages and adds the new struct to it.
    __mm_add_struct_to_global_families(struct_name, struct_size);
}

int main(int argc, char **argv) {
    mm_init();
    
    __uint32_t size = 4096;
    for(int i = 0; i <= 120; i++) {
        char struct_name[64];  // tamanho suficiente
        snprintf(struct_name, sizeof(struct_name), "test_t%d", i);
        mm_instantiate_new_page_family(struct_name, size); 
    }
    
    printf("terminou");
    
    return 0;
}