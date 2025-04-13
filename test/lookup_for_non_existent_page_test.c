#include "../include/memory_manager_uapi.h"
#include <assert.h>
#include <string.h>

// Test:
static void lookup_for_non_existent_struct_test();

// To run this test you should change the name of the function to main
int run_test(void) {
    lookup_for_non_existent_struct_test();
    
    return 0;
}


static void lookup_for_non_existent_struct_test() {
    // Init:
    mm_init();

    // Given:
    // None structure registered

    // Then:
    // Should return vm_page_family_t->name == equals test_t70.
    char *struct_name = "test_t70";
    vm_page_family_t *page = lookup_page_family_by_name(struct_name);
    assert(NULL == page);
    printf("Expected: %s \nActual: %s \nTest passed! \n", NULL, page);
}