#include "../include/memory_manager_uapi.h"

// To run this test you should change the name of the function to main
int run_test(void) {
    register_same_struct_family_twice_test();
    
    return 0;
}


static void register_same_struct_family_twice_test() {
    // Init:
    mm_init();

    // Given:
    MM_REGISTER_STRUCT("struct_t");
    MM_REGISTER_STRUCT("struct_t");

    // Then:
    // Should log "Struct struct_t is already cached." and Assert(0).
}