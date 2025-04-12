#include "../include/memory_manager_uapi.h"
#include "../include/memory_manager.h"
#include <stdio.h>

void mm_print_registered_page_families() {
    vm_page_for_families_t *page_families_iter = get_families_head();
    vm_page_family_t *page_family_iter = NULL;
    int count = 1;
    while (TRUE) {
        printf("Family n %i structs: \n", count);
        ITERATE_PAGE_FAMILIES_BEGIN(page_families_iter, page_family_iter) {
            printf("%s - ", page_family_iter->struct_name);
        } ITERATE_PAGE_FAMILIES_END()

        printf("\n\n");

        if (page_families_iter->next == NULL) {
            break;
        }

        page_families_iter = page_families_iter->next;
        count++;
    }
}