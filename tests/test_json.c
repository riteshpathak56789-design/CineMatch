#include "json_parser.h"
#include <stdio.h>
#include <string.h>

static int g_json_tests_run = 0;
static int g_json_tests_passed = 0;

#define TEST_ASSERT(cond, msg) do { \
    g_json_tests_run++; \
    if (cond) { \
        g_json_tests_passed++; \
    } else { \
        printf("  [FAIL] %s (Line %d): %s\n", __FILE__, __LINE__, msg); \
    } \
} while(0)

static const char *MOCK_POPULAR_JSON =
"{\n"
"  \"page\": 1,\n"
"  \"results\": [\n"
"    {\n"
"      \"id\": 550,\n"
"      \"title\": \"Fight Club\",\n"
"      \"original_language\": \"en\",\n"
"      \"release_date\": \"1999-10-15\",\n"
"      \"vote_average\": 8.433,\n"
"      \"genre_ids\": [18, 53],\n"
"      \"overview\": \"A ticking-time-bomb insomniac and a slippery soap salesman build a global organization.\",\n"
"      \"poster_path\": \"/pB8BM7pdSp6B6Ih7QZ4DrQ3PmJK.jpg\"\n"
"    },\n"
"    {\n"
"      \"id\": 27205,\n"
"      \"title\": \"Inception\",\n"
"      \"original_language\": \"en\",\n"
"      \"release_date\": \"2010-07-15\",\n"
"      \"vote_average\": 8.364,\n"
"      \"genre_ids\": [28, 878, 12],\n"
"      \"overview\": \"Cobb, a skilled thief who commits corporate espionage by infiltrating the subconscious.\",\n"
"      \"poster_path\": \"/edv5CZvWj09upOsy2Y6IwDhK8bt.jpg\"\n"
"    }\n"
"  ],\n"
"  \"total_pages\": 500,\n"
"  \"total_results\": 10000\n"
"}";

static const char *MOCK_NULL_FIELDS_JSON =
"{\n"
"  \"page\": 1,\n"
"  \"results\": [\n"
"    {\n"
"      \"id\": 9999,\n"
"      \"title\": \"Unreleased Mystery Movie\",\n"
"      \"original_language\": \"hi\",\n"
"      \"release_date\": null,\n"
"      \"vote_average\": 0.0,\n"
"      \"genre_ids\": [],\n"
"      \"overview\": null,\n"
"      \"poster_path\": null\n"
"    }\n"
"  ],\n"
"  \"total_pages\": 1,\n"
"  \"total_results\": 1\n"
"}";

static const char *MOCK_DETAILS_JSON =
"{\n"
"  \"id\": 550,\n"
"  \"title\": \"Fight Club\",\n"
"  \"original_language\": \"en\",\n"
"  \"release_date\": \"1999-10-15\",\n"
"  \"vote_average\": 8.4,\n"
"  \"genres\": [\n"
"    {\"id\": 18, \"name\": \"Drama\"},\n"
"    {\"id\": 53, \"name\": \"Thriller\"}\n"
"  ],\n"
"  \"overview\": \"A ticking-time-bomb insomniac...\",\n"
"  \"poster_path\": \"/pB8BM7pdSp6B6Ih7QZ4DrQ3PmJK.jpg\"\n"
"}";

static const char *MOCK_ERROR_JSON =
"{\n"
"  \"status_code\": 7,\n"
"  \"status_message\": \"Invalid API key: You must be granted a valid key.\",\n"
"  \"success\": false\n"
"}";

void test_parse_movie_list(void) {
    printf("Testing JSON parsing of paginated MovieList...\n");
    MovieList list;
    JsonParseResult res = json_parse_movie_list(MOCK_POPULAR_JSON, &list);

    TEST_ASSERT(res == JSON_PARSE_SUCCESS, "Parse popular JSON succeeded");
    TEST_ASSERT(list.count == 2, "Parsed 2 movie results");
    TEST_ASSERT(list.page == 1, "Page is 1");
    TEST_ASSERT(list.total_pages == 500, "Total pages is 500");
    TEST_ASSERT(list.total_results == 10000, "Total results is 10000");

    /* Movie 1 */
    TEST_ASSERT(list.items[0].id == 550, "Movie 0 ID is 550");
    TEST_ASSERT(strcmp(list.items[0].title, "Fight Club") == 0, "Movie 0 title is Fight Club");
    TEST_ASSERT(strcmp(list.items[0].original_language, "en") == 0, "Movie 0 lang is en");
    TEST_ASSERT(strcmp(list.items[0].release_date, "1999-10-15") == 0, "Movie 0 date is 1999-10-15");
    TEST_ASSERT(list.items[0].rating > 8.4 && list.items[0].rating < 8.5, "Movie 0 rating is ~8.43");
    TEST_ASSERT(list.items[0].genre_count == 2, "Movie 0 genre count is 2");
    TEST_ASSERT(list.items[0].genre_ids[0] == 18, "Movie 0 genre 0 is 18 (Drama)");
    TEST_ASSERT(list.items[0].genre_ids[1] == 53, "Movie 0 genre 1 is 53 (Thriller)");

    /* Movie 2 */
    TEST_ASSERT(list.items[1].id == 27205, "Movie 1 ID is 27205");
    TEST_ASSERT(strcmp(list.items[1].title, "Inception") == 0, "Movie 1 title is Inception");
    TEST_ASSERT(list.items[1].genre_count == 3, "Movie 1 genre count is 3");
}

void test_parse_null_fields(void) {
    printf("Testing defensive parsing of null/empty fields...\n");
    MovieList list;
    JsonParseResult res = json_parse_movie_list(MOCK_NULL_FIELDS_JSON, &list);

    TEST_ASSERT(res == JSON_PARSE_SUCCESS, "Parse JSON with null fields succeeded");
    TEST_ASSERT(list.count == 1, "Parsed 1 item");
    TEST_ASSERT(list.items[0].id == 9999, "Movie ID is 9999");
    TEST_ASSERT(strcmp(list.items[0].release_date, "N/A") == 0, "Null release_date converted to N/A");
    TEST_ASSERT(strcmp(list.items[0].poster_path, "N/A") == 0, "Null poster_path converted to N/A");
    TEST_ASSERT(list.items[0].genre_count == 0, "Empty genre_ids has count 0");
}

void test_parse_single_movie_details(void) {
    printf("Testing JSON parsing of Movie Details with genre objects...\n");
    Movie m;
    JsonParseResult res = json_parse_single_movie(MOCK_DETAILS_JSON, &m);

    TEST_ASSERT(res == JSON_PARSE_SUCCESS, "Parse single movie succeeded");
    TEST_ASSERT(m.id == 550, "Movie details ID is 550");
    TEST_ASSERT(m.genre_count == 2, "Parsed 2 genre objects into IDs");
    TEST_ASSERT(m.genre_ids[0] == 18, "Genre 0 is 18");
    TEST_ASSERT(m.genre_ids[1] == 53, "Genre 1 is 53");
}

void test_parse_error_payload(void) {
    printf("Testing TMDB error message extraction...\n");
    char err_buf[128];
    bool found = json_parse_tmdb_error(MOCK_ERROR_JSON, err_buf, sizeof(err_buf));

    TEST_ASSERT(found == true, "Found status_message in error payload");
    TEST_ASSERT(strstr(err_buf, "Invalid API key") != NULL, "Extracted correct error message text");
}

void test_malformed_json(void) {
    printf("Testing resilience to malformed JSON...\n");
    MovieList list;
    JsonParseResult res = json_parse_movie_list("{\"results\": [ { \"id\": 1, \"title\": ", &list);
    TEST_ASSERT(res != JSON_PARSE_SUCCESS, "Malformed JSON correctly rejected with error code");
}

int run_json_tests(void) {
    g_json_tests_run = 0;
    g_json_tests_passed = 0;
    test_parse_movie_list();
    test_parse_null_fields();
    test_parse_single_movie_details();
    test_parse_error_payload();
    test_malformed_json();
    printf("JSON Suite: %d / %d tests passed.\n", g_json_tests_passed, g_json_tests_run);
    return (g_json_tests_run == g_json_tests_passed) ? 0 : 1;
}
