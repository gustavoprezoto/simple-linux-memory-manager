#include "../include/memory_manager_uapi.h"

// Test case:
static void register_3_families_pages_of_structs_test() {
    // Init:
    mm_init();

    // Given 120 families:
    __mock_families_for_test();

    // Then should print three pages of struct families (expected text can be found in expected.txt): 
    mm_print_registered_page_families();
}

// Utils:
static void __mock_families_for_test();

// To run this test you should change the name of the function to main
int run_test(void) {
    register_3_families_pages_of_structs_test();

    return 0; 
}

static void __mock_families_for_test() {
    __uint32_t size = 4096;
    for(int i = 1; i <= (60*3); i++) {
        char struct_name[64];
        snprintf(struct_name, sizeof(struct_name), "test_t%d", i);
        // MM_REGISTER_STRUCT(struct_name); // This macro cannot use variables to register structs, so im unwrapping it:
        mm_instantiate_new_page_family(struct_name, size);
    }
}
