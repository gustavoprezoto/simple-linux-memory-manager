#ifndef UAPI_MM_H
#define UAPI_MM_H

#include "memory_manager.h"

#define MM_REGISTER_STRUCT(struct_name) \
    (mm_instantiate_new_page_family(#struct_name, sizeof(struct_name)))

/**
 * @brief This function uses std printf() function to print all page families.
 *
 */
void mm_print_registered_page_families();

/**
 * @brief This function is used to find the a page_family in among all 
 * global pages previously registered.
 *
 * @param *struct_name A string containing the struct_name to be found.
 * 
 * @returns A pointer to the struct vm_page_family_t previously registered.
 */
vm_page_family_t *lookup_page_family_by_name(char *struct_name);


#endif