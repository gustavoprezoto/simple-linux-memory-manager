#include "../include/memory_manager_uapi.h"
#include <assert.h>
#include <string.h>

// Test:
static void lookup_for_non_existent_struct_test();

// Utils:
void register_n_different_structures(int n);

// To run this test you should change the name of the function to main
int run_test(void) {
    lookup_for_non_existent_struct_test();
    
    return 0;
}


static void lookup_for_non_existent_struct_test() {
    // Init:
    mm_init();

    // Given:
    // 180 different structures registered
    register_n_different_structures(180);

    // Then:
    // Should return vm_page_family_t->name == equals test_t70.
    char *expectedName = "test_t70";
    vm_page_family_t *page = lookup_page_family_by_name(expectedName);
    assert(strcmp(expectedName, page->struct_name) == 0);
    printf("Expected: %s \nActual: %s\nTest passed!\n", expectedName, page->struct_name);
}

void register_n_different_structures(int n) {
    __uint32_t size = 4096;
    for(int i = 1; i <= (n); i++) {
        char struct_name[64];
        snprintf(struct_name, sizeof(struct_name), "test_t%d", i);
        // MM_REGISTER_STRUCT(struct_name); // This macro cannot use variables to register structs, so im unwrapping it:
        mm_instantiate_new_page_family(struct_name, size);
    }
}