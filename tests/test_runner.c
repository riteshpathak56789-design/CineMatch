#include <stdio.h>
#include "tmdb.h"

int run_ds_tests(void);
int run_json_tests(void);
int run_tmdb_tests(void);

int main(void) {
    printf("====================================================\n");
    printf("           CINEMATCH AUTOMATED TEST SUITE           \n");
    printf("====================================================\n\n");

    if (!tmdb_global_init()) {
        fprintf(stderr, "Failed to initialize libcurl for test suite.\n");
        return 1;
    }

    int failures = 0;

    printf("--- [1/3] Running Data Structures & Algorithms Suite ---\n");
    failures += run_ds_tests();
    printf("\n");

    printf("--- [2/3] Running JSON Parser & Model Suite ---\n");
    failures += run_json_tests();
    printf("\n");

    printf("--- [3/3] Running TMDB Client & Network Suite ---\n");
    failures += run_tmdb_tests();
    printf("\n");

    tmdb_global_cleanup();

    printf("====================================================\n");
    if (failures == 0) {
        printf("  ALL TEST SUITES PASSED SUCCESSFULLY (100%%)       \n");
        printf("====================================================\n");
        return 0;
    } else {
        printf("  TEST FAILURES DETECTED: %d suite(s) failed       \n", failures);
        printf("====================================================\n");
        return 1;
    }
}
