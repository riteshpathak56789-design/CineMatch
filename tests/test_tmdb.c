#include "tmdb.h"
#include <stdio.h>
#include <string.h>

static int g_tmdb_tests_run = 0;
static int g_tmdb_tests_passed = 0;

#define TEST_ASSERT(cond, msg) do { \
    g_tmdb_tests_run++; \
    if (cond) { \
        g_tests_passed_count(); \
    } else { \
        printf("  [FAIL] %s (Line %d): %s\n", __FILE__, __LINE__, msg); \
    } \
} while(0)

static void g_tests_passed_count(void) {
    g_tmdb_tests_passed++;
}

void test_url_encoding(void) {
    printf("Testing URL encoding...\n");
    char out[128];

    bool ok = tmdb_url_encode("The Dark Knight", out, sizeof(out));
    TEST_ASSERT(ok == true, "URL encode 'The Dark Knight'");
    TEST_ASSERT(strcmp(out, "The%20Dark%20Knight") == 0, "Spaces encoded as %20");

    ok = tmdb_url_encode("Fast & Furious 9", out, sizeof(out));
    TEST_ASSERT(ok == true, "URL encode with '&'");
    TEST_ASSERT(strcmp(out, "Fast%20%26%20Furious%209") == 0, "Ampersand encoded as %26");

    ok = tmdb_url_encode("SimpleTitle", out, sizeof(out));
    TEST_ASSERT(ok == true, "URL encode simple alphanumeric");
    TEST_ASSERT(strcmp(out, "SimpleTitle") == 0, "Alphanumeric unchanged");
}

void test_error_strings(void) {
    printf("Testing error code descriptions...\n");
    const char *msg_auth = tmdb_result_to_string(TMDB_ERR_UNAUTHORIZED);
    TEST_ASSERT(strstr(msg_auth, "Authentication") != NULL, "Auth error string contains 'Authentication'");

    const char *msg_notfound = tmdb_result_to_string(TMDB_ERR_NOT_FOUND);
    TEST_ASSERT(strstr(msg_notfound, "404") != NULL, "Not found string contains '404'");

    const char *msg_token = tmdb_result_to_string(TMDB_ERR_NO_TOKEN);
    TEST_ASSERT(strstr(msg_token, "TMDB_BEARER_TOKEN") != NULL, "No token string mentions TMDB_BEARER_TOKEN");
}

void test_tmdb_unauthenticated_ping(void) {
    printf("Testing unauthenticated TMDB HTTPS TLS connection...\n");
    int http_code = 0;
    TmdbResult res = tmdb_ping_unauthenticated(&http_code);

    /* Unauthenticated call to /configuration should reach TMDB and receive HTTP 401 Unauthorized */
    TEST_ASSERT(http_code == 401, "TMDB endpoint /configuration responded with HTTP 401 Unauthorized as expected without credentials");
    TEST_ASSERT(res == TMDB_ERR_UNAUTHORIZED, "Result mapped to TMDB_ERR_UNAUTHORIZED");
}

int run_tmdb_tests(void) {
    g_tmdb_tests_run = 0;
    g_tmdb_tests_passed = 0;
    test_url_encoding();
    test_error_strings();
    test_tmdb_unauthenticated_ping();
    printf("TMDB Client Suite: %d / %d tests passed.\n", g_tmdb_tests_passed, g_tmdb_tests_run);
    return (g_tmdb_tests_run == g_tmdb_tests_passed) ? 0 : 1;
}
