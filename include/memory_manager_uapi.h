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


#endif