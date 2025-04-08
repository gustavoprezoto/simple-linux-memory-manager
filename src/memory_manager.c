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
             __count < MM_MAX_FAMILIES_PER_VM_PAGE &&                       \
             vm_page_for_families_ptr->vm_page_family[__count].struct_size; \
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
#define ITERATE_PAGE_FAMILIES_END(vm_page_for_families_ptr, curr) \
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
static vm_page_for_families_t* __mm_create_new_page_for_families_struct(char *struct_name, __uint32_t struct_size) {
    vm_page_for_families_t *page = (vm_page_for_families_t*) __mm_get_new_vm_pages_from_kernel(1);

    // Creates the first page family entry
    strncpy(page->vm_page_family[0].struct_name, struct_name, sizeof(page->vm_page_family[0].struct_name));
    page->vm_page_family[0].struct_size = struct_size;

    page->next = NULL;

    return page;
}

static size_t __is_vm_page_for_families_full(vm_page_for_families_t *page) {
    printf("test");
    vm_page_family_t *family = NULL;

    // todo: This function can be improved using a flag in the struct
    for (size_t i; i < MM_MAX_FAMILIES_PER_VM_PAGE; i++) {
        if (page->vm_page_family[i].struct_size) {
            return FALSE;
        }
    }

    return TRUE;
}

static void __mm_add_new_family_group_page_to_global_group(vm_page_for_families_t *new_page) {
    last_family_group_page->next = new_page;
    last_family_group_page = new_page;
}

static void __mm_add_struct_to_global_families(char *struct_name, __uint32_t struct_size) {
    vm_page_for_families_t *iter = first_family_group_page;
    
    while(iter != NULL) {
        for (__uint32_t i = 0; i < MM_MAX_FAMILIES_PER_VM_PAGE; i++) {
            
        }
    }
    // i need to iterate for each vm_page_for_families_t entry
    // while (1) {
    //     if (__is_vm_page_for_families_full(first_family_group_page)) {
    //         vm_page_for_families_t *new_family = __mm_create_new_page_for_families_struct(struct_name, struct_size);
    //         __mm_add_new_family_group_page_to_global_group(new_family);
    //     }

    //     if ()
    // }

    // for (__uint32_t i = 0; i < MM_MAX_FAMILIES_PER_VM_PAGE; i++) {
        
    // }

    // if (__is_vm_page_for_families_full(first_family_group_page)) {
    //     vm_page_for_families_t *new_family = __mm_create_new_page_for_families_struct(struct_name, struct_size);
    //     __mm_add_new_family_group_page_to_global_group(new_family);
    //     return;
    // }


}

void mm_instantiate_new_page_family(char *struct_name, __uint32_t struct_size) {
    if (struct_size > SYSTEM_PAGE_SIZE) {
        printf("Error: Struct (%s) size is bigger than accepted by an OS page. Skipping.\n", struct_name);
        return;
    }

    if (!first_family_group_page) {
        first_family_group_page = __mm_create_new_page_for_families_struct(struct_name, struct_size);
        last_family_group_page = first_family_group_page;
        return;
    }

    __mm_add_struct_to_global_families(struct_name, struct_size);
}

int main(int argc, char **argv) {
    mm_init();
    
    char *strx = "test_t";
    __uint32_t size = 4096;

    char *stry = "test2_t";
    __uint32_t sizey = 4096;
    
    mm_instantiate_new_page_family(strx, size);
    mm_instantiate_new_page_family(stry, sizey);
    
    printf("terminou");
    
    return 0;
}