#include "ds.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

static int g_tests_run = 0;
static int g_tests_passed = 0;

#define TEST_ASSERT(cond, msg) do { \
    g_tests_run++; \
    if (cond) { \
        g_tests_passed++; \
    } else { \
        printf("  [FAIL] %s (Line %d): %s\n", __FILE__, __LINE__, msg); \
    } \
} while(0)

void test_hash_table(void) {
    printf("Testing Hash Table operations...\n");
    HashTable ht;
    hash_table_init(&ht);
    TEST_ASSERT(ht.size == 0, "Hash table initialized with size 0");

    /* Insert keys */
    TEST_ASSERT(hash_table_insert(&ht, 550, 0), "Insert key 550");
    TEST_ASSERT(hash_table_insert(&ht, 680, 1), "Insert key 680");
    TEST_ASSERT(hash_table_insert(&ht, 120, 2), "Insert key 120");
    TEST_ASSERT(ht.size == 3, "Hash table size is 3");

    /* Find existing */
    TEST_ASSERT(hash_table_find(&ht, 550) == 0, "Find key 550 -> index 0");
    TEST_ASSERT(hash_table_find(&ht, 680) == 1, "Find key 680 -> index 1");
    TEST_ASSERT(hash_table_find(&ht, 120) == 2, "Find key 120 -> index 2");

    /* Find non-existing */
    TEST_ASSERT(hash_table_find(&ht, 99999) == -1, "Find non-existent key returns -1");

    /* Update existing key */
    TEST_ASSERT(hash_table_insert(&ht, 550, 99), "Update key 550 with index 99");
    TEST_ASSERT(hash_table_find(&ht, 550) == 99, "Find updated key 550 -> index 99");
    TEST_ASSERT(ht.size == 3, "Size remains 3 after update");

    /* Remove existing key */
    TEST_ASSERT(hash_table_remove(&ht, 680), "Remove key 680 succeeds");
    TEST_ASSERT(hash_table_find(&ht, 680) == -1, "Find removed key 680 returns -1");
    TEST_ASSERT(ht.size == 2, "Size decremented to 2");

    /* Remove non-existing key */
    TEST_ASSERT(!hash_table_remove(&ht, 99999), "Remove non-existent key returns false");

    /* Clear */
    hash_table_clear(&ht);
    TEST_ASSERT(ht.size == 0, "Hash table cleared, size 0");
    TEST_ASSERT(hash_table_find(&ht, 550) == -1, "Find after clear returns -1");
}

void test_graph_and_traversals(void) {
    printf("Testing Graph and BFS/DFS Traversals...\n");
    Graph g;
    graph_init(&g, 4);
    TEST_ASSERT(g.num_vertices == 4, "Graph initialized with 4 vertices");

    /* Build a connected triangle: 0 - 1 - 2 - 0, and a branch 1 - 3 */
    graph_add_edge(&g, 0, 1, 2);
    graph_add_edge(&g, 1, 2, 1);
    graph_add_edge(&g, 2, 0, 3);
    graph_add_edge(&g, 1, 3, 1);

    /* Test BFS starting at 0: should visit 0, then immediate neighbors (1, 2), then 3 */
    int bfs_order[4];
    int bfs_count = graph_bfs(&g, 0, bfs_order, 4);
    TEST_ASSERT(bfs_count == 4, "BFS visited all 4 reachable vertices");
    TEST_ASSERT(bfs_order[0] == 0, "BFS start vertex is 0");
    TEST_ASSERT((bfs_order[1] == 1 || bfs_order[1] == 2), "BFS first neighbor is 1 or 2");
    TEST_ASSERT((bfs_order[2] == 1 || bfs_order[2] == 2), "BFS second neighbor is 1 or 2");
    TEST_ASSERT(bfs_order[3] == 3, "BFS last visited vertex is 3 (level 2)");

    /* Test DFS starting at 0 */
    int dfs_order[4];
    int dfs_count = graph_dfs(&g, 0, dfs_order, 4);
    TEST_ASSERT(dfs_count == 4, "DFS visited all 4 vertices");
    TEST_ASSERT(dfs_order[0] == 0, "DFS start vertex is 0");

    /* Clean up */
    graph_free(&g);
    TEST_ASSERT(g.num_vertices == 0, "Graph freed, vertices 0");
}

void test_sorting_algorithms(void) {
    printf("Testing Sorting Algorithms...\n");
    MovieList list;
    movie_list_init(&list);
    list.count = 4;

    list.items[0].id = 1;
    strcpy(list.items[0].title, "Zeta");
    strcpy(list.items[0].release_date, "2015-05-10");
    list.items[0].rating = 7.2;

    list.items[1].id = 2;
    strcpy(list.items[1].title, "Alpha");
    strcpy(list.items[1].release_date, "2020-01-01");
    list.items[1].rating = 8.9;

    list.items[2].id = 3;
    strcpy(list.items[2].title, "Gamma");
    strcpy(list.items[2].release_date, "2018-08-15");
    list.items[2].rating = 6.5;

    list.items[3].id = 4;
    strcpy(list.items[3].title, "Beta");
    strcpy(list.items[3].release_date, "2022-11-20");
    list.items[3].rating = 8.1;

    /* Sort by rating descending (Insertion Sort) */
    sort_movies_by_rating_desc(&list);
    TEST_ASSERT(list.items[0].rating >= list.items[1].rating, "Rating 0 >= 1");
    TEST_ASSERT(list.items[1].rating >= list.items[2].rating, "Rating 1 >= 2");
    TEST_ASSERT(list.items[2].rating >= list.items[3].rating, "Rating 2 >= 3");
    TEST_ASSERT(list.items[0].id == 2, "Top rated movie is Alpha (8.9)");
    TEST_ASSERT(list.items[3].id == 3, "Lowest rated movie is Gamma (6.5)");

    /* Sort by release date descending (Selection Sort) */
    sort_movies_by_release_date_desc(&list);
    TEST_ASSERT(strcmp(list.items[0].release_date, "2022-11-20") == 0, "Newest release date is 2022-11-20");
    TEST_ASSERT(strcmp(list.items[3].release_date, "2015-05-10") == 0, "Oldest release date is 2015-05-10");

    /* Sort by title ascending (Selection Sort) */
    sort_movies_by_title_asc(&list);
    TEST_ASSERT(strcmp(list.items[0].title, "Alpha") == 0, "Alphabetical 1st: Alpha");
    TEST_ASSERT(strcmp(list.items[1].title, "Beta") == 0, "Alphabetical 2nd: Beta");
    TEST_ASSERT(strcmp(list.items[2].title, "Gamma") == 0, "Alphabetical 3rd: Gamma");
    TEST_ASSERT(strcmp(list.items[3].title, "Zeta") == 0, "Alphabetical 4th: Zeta");
}

void test_searching_and_filtering(void) {
    printf("Testing Searching & Filtering...\n");
    MovieList src, dest;
    movie_list_init(&src);
    src.count = 3;

    src.items[0].id = 1;
    strcpy(src.items[0].title, "The Dark Knight");
    strcpy(src.items[0].original_language, "en");
    src.items[0].genre_ids[0] = 28; /* Action */
    src.items[0].genre_ids[1] = 80; /* Crime */
    src.items[0].genre_count = 2;

    src.items[1].id = 2;
    strcpy(src.items[1].title, "Inception");
    strcpy(src.items[1].original_language, "en");
    src.items[1].genre_ids[0] = 28; /* Action */
    src.items[1].genre_ids[1] = 878; /* Sci-Fi */
    src.items[1].genre_count = 2;

    src.items[2].id = 3;
    strcpy(src.items[2].title, "Spirited Away");
    strcpy(src.items[2].original_language, "ja");
    src.items[2].genre_ids[0] = 16; /* Animation */
    src.items[2].genre_ids[1] = 14; /* Fantasy */
    src.items[2].genre_count = 2;

    /* Substring search */
    filter_movies_by_title(&src, &dest, "dark");
    TEST_ASSERT(dest.count == 1, "Search 'dark' matched 1 movie");
    TEST_ASSERT(dest.items[0].id == 1, "Matched The Dark Knight");

    filter_movies_by_title(&src, &dest, "InCePtIoN");
    TEST_ASSERT(dest.count == 1, "Case-insensitive search matched 1 movie");
    TEST_ASSERT(dest.items[0].id == 2, "Matched Inception");

    /* Genre filtering */
    filter_movies_by_genre(&src, &dest, 28); /* Action */
    TEST_ASSERT(dest.count == 2, "Genre Action (28) matched 2 movies");

    filter_movies_by_genre(&src, &dest, 16); /* Animation */
    TEST_ASSERT(dest.count == 1, "Genre Animation (16) matched 1 movie");
    TEST_ASSERT(dest.items[0].id == 3, "Matched Spirited Away");

    /* Language filtering */
    filter_movies_by_language(&src, &dest, "ja");
    TEST_ASSERT(dest.count == 1, "Language 'ja' matched 1 movie");
    TEST_ASSERT(dest.items[0].id == 3, "Matched Spirited Away");

    /* Genre overlap calculation */
    int shared = calculate_shared_genres(&src.items[0], &src.items[1]);
    TEST_ASSERT(shared == 1, "The Dark Knight and Inception share 1 genre (Action)");
    int no_shared = calculate_shared_genres(&src.items[0], &src.items[2]);
    TEST_ASSERT(no_shared == 0, "The Dark Knight and Spirited Away share 0 genres");
}

int run_ds_tests(void) {
    g_tests_run = 0;
    g_tests_passed = 0;
    test_hash_table();
    test_graph_and_traversals();
    test_sorting_algorithms();
    test_searching_and_filtering();
    printf("DS Suite: %d / %d tests passed.\n", g_tests_passed, g_tests_run);
    return (g_tests_run == g_tests_passed) ? 0 : 1;
}
