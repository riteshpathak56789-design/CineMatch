#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "movie.h"
#include "ds.h"
#include "tmdb.h"
#include "json_parser.h"
#include "ui.h"

static MovieList g_movies;
static HashTable g_hash_table;
static Graph g_graph;
static int g_current_page = 1;
static char g_last_query[128] = "";
static char g_api_buffer[TMDB_MAX_RESPONSE_SIZE];

/* Refresh Data Structures after loading new movies */
static void update_data_structures(void) {
    hash_table_build_from_list(&g_hash_table, &g_movies);
    graph_build_from_genres(&g_graph, &g_movies);
}

/* Helper to process raw JSON into g_movies and notify user */
static bool process_json_response(const char *json_data, const char *action_desc) {
    JsonParseResult parse_res = json_parse_movie_list(json_data, &g_movies);
    if (parse_res != JSON_PARSE_SUCCESS) {
        char err_msg[128];
        if (json_parse_tmdb_error(json_data, err_msg, sizeof(err_msg))) {
            printf("\n%s[TMDB Error]%s %s\n", ANSI_RED, ANSI_RESET, err_msg);
        } else {
            printf("\n%s[Error]%s Failed to parse movie response (Code %d).\n",
                   ANSI_RED, ANSI_RESET, parse_res);
        }
        return false;
    }

    if (g_movies.count == 0) {
        printf("\n%s[Notice]%s %s returned 0 matching results.\n",
               ANSI_YELLOW, ANSI_RESET, action_desc);
        return false;
    }

    update_data_structures();
    printf("\n%s[Success]%s Loaded %d movies from TMDB.\n",
           ANSI_GREEN, ANSI_RESET, g_movies.count);
    return true;
}

static void handle_discover_popular(int page) {
    ui_clear_screen();
    ui_print_header("Discover Popular Movies");
    printf("\nFetching page %d of popular movies from TMDB...\n", page);

    int http_code = 0;
    TmdbResult res = tmdb_fetch_popular(page, g_api_buffer, sizeof(g_api_buffer), &http_code);
    if (res != TMDB_SUCCESS) {
        printf("\n%s[Error]%s %s (HTTP Status: %d)\n",
               ANSI_RED, ANSI_RESET, tmdb_result_to_string(res), http_code);
        ui_pause_for_user();
        return;
    }

    g_current_page = page;
    g_last_query[0] = '\0';
    if (process_json_response(g_api_buffer, "Popular movies query")) {
        ui_display_movie_table(&g_movies);
    }
    ui_pause_for_user();
}

static void handle_search(int page) {
    ui_clear_screen();
    ui_print_header("Search Movies by Title");

    char query[128];
    if (page == 1) {
        if (!ui_read_string("Enter movie title to search: ", query, sizeof(query))) {
            printf("%sSearch cancelled (empty title).%s\n", ANSI_YELLOW, ANSI_RESET);
            ui_pause_for_user();
            return;
        }
        strncpy(g_last_query, query, sizeof(g_last_query));
    } else {
        strncpy(query, g_last_query, sizeof(query));
    }

    printf("\nSearching TMDB for \"%s\" (page %d)...\n", query, page);

    int http_code = 0;
    TmdbResult res = tmdb_fetch_search(query, page, g_api_buffer, sizeof(g_api_buffer), &http_code);
    if (res != TMDB_SUCCESS) {
        printf("\n%s[Error]%s %s (HTTP Status: %d)\n",
               ANSI_RED, ANSI_RESET, tmdb_result_to_string(res), http_code);
        ui_pause_for_user();
        return;
    }

    g_current_page = page;
    if (process_json_response(g_api_buffer, "Title search")) {
        ui_display_movie_table(&g_movies);
    }
    ui_pause_for_user();
}

static void handle_filter_genre_lang(void) {
    ui_clear_screen();
    ui_print_header("Filter Movies by Genre and Language");

    printf("Popular Genre Options:\n");
    printf("  [28] Action       [35] Comedy       [18] Drama\n");
    printf("  [878] Sci-Fi      [27] Horror       [53] Thriller\n");
    printf("  [16] Animation    [10749] Romance   [0] Any Genre\n\n");

    int genre_id = 0;
    ui_read_int("Enter Genre ID (0 for all)", &genre_id, 0, 99999);

    char lang[16];
    printf("Enter Language ISO code (e.g. en, hi, ja, ko, fr) or press [Enter] for all: ");
    fflush(stdout);
    if (!fgets(lang, sizeof(lang), stdin)) {
        lang[0] = '\0';
    }
    size_t len = strlen(lang);
    while (len > 0 && (lang[len - 1] == '\n' || lang[len - 1] == '\r')) {
        lang[--len] = '\0';
    }

    printf("\nQuerying TMDB discover endpoint...\n");
    int http_code = 0;
    TmdbResult res = tmdb_fetch_discover(genre_id, lang, 1, g_api_buffer, sizeof(g_api_buffer), &http_code);
    if (res != TMDB_SUCCESS) {
        printf("\n%s[Error]%s %s (HTTP Status: %d)\n",
               ANSI_RED, ANSI_RESET, tmdb_result_to_string(res), http_code);
        ui_pause_for_user();
        return;
    }

    g_current_page = 1;
    g_last_query[0] = '\0';
    if (process_json_response(g_api_buffer, "Discover filter")) {
        ui_display_movie_table(&g_movies);
    }
    ui_pause_for_user();
}

static void handle_sort(void) {
    ui_clear_screen();
    ui_print_header("Sort Current Results");

    if (g_movies.count == 0) {
        printf("\n%s[Notice]%s No movies loaded. Please search or discover movies first.\n",
               ANSI_YELLOW, ANSI_RESET);
        ui_pause_for_user();
        return;
    }

    printf("Select Sorting Algorithm:\n");
    printf("  [1] Insertion Sort: Sort by Rating (High -> Low)\n");
    printf("  [2] Selection Sort: Sort by Release Date (Newest -> Oldest)\n");
    printf("  [3] Selection Sort: Sort by Title (Alphabetical A -> Z)\n");
    printf("  [0] Cancel\n\n");

    int choice = 0;
    if (!ui_read_int("Enter sorting option", &choice, 0, 3) || choice == 0) {
        return;
    }

    switch (choice) {
        case 1:
            sort_movies_by_rating_desc(&g_movies);
            update_data_structures();
            printf("\n%s[Done]%s Applied Insertion Sort by Rating.\n", ANSI_GREEN, ANSI_RESET);
            break;
        case 2:
            sort_movies_by_release_date_desc(&g_movies);
            update_data_structures();
            printf("\n%s[Done]%s Applied Selection Sort by Release Date.\n", ANSI_GREEN, ANSI_RESET);
            break;
        case 3:
            sort_movies_by_title_asc(&g_movies);
            update_data_structures();
            printf("\n%s[Done]%s Applied Selection Sort by Title.\n", ANSI_GREEN, ANSI_RESET);
            break;
    }

    ui_display_movie_table(&g_movies);
    ui_pause_for_user();
}

static void handle_view_details(void) {
    ui_clear_screen();
    ui_print_header("View Movie Details");

    if (g_movies.count == 0) {
        printf("\n%s[Notice]%s No movies currently loaded.\n", ANSI_YELLOW, ANSI_RESET);
        ui_pause_for_user();
        return;
    }

    ui_display_movie_table(&g_movies);

    int row = 0;
    if (!ui_read_int("Select row number [1 - N] or 0 to cancel", &row, 0, g_movies.count) || row == 0) {
        return;
    }

    Movie *m = &g_movies.items[row - 1];

    /* Fetch live details from TMDB to get full overview and details */
    int http_code = 0;
    TmdbResult res = tmdb_fetch_details(m->id, g_api_buffer, sizeof(g_api_buffer), &http_code);
    if (res == TMDB_SUCCESS) {
        Movie detailed;
        if (json_parse_single_movie(g_api_buffer, &detailed) == JSON_PARSE_SUCCESS) {
            *m = detailed;
        }
    }

    ui_display_movie_card(m);
    ui_pause_for_user();
}

static void handle_recommendations_graph(void) {
    ui_clear_screen();
    ui_print_header("Movie Recommendations & Graph Traversal");

    if (g_movies.count < 2) {
        printf("\n%s[Notice]%s At least 2 movies are needed to demonstrate graph relationships.\n",
               ANSI_YELLOW, ANSI_RESET);
        ui_pause_for_user();
        return;
    }

    ui_display_graph_structure(&g_graph, &g_movies);
    ui_display_movie_table(&g_movies);

    int seed_row = 0;
    if (!ui_read_int("Select seed movie [1 - N] to explore recommendations", &seed_row, 1, g_movies.count)) {
        return;
    }

    int seed_idx = seed_row - 1;
    Movie *seed = &g_movies.items[seed_idx];
    printf("\nSelected Seed: %s%s%s (TMDB ID: %d)\n\n",
           ANSI_BOLD, seed->title, ANSI_RESET, seed->id);

    printf("Choose Recommendation Exploration Method:\n");
    printf("  [1] Breadth-First Search (BFS) over Shared-Genre Graph\n");
    printf("  [2] Depth-First Search (DFS) over Recommendation Chain\n");
    printf("  [3] Fetch Live TMDB Recommendations from API\n");
    printf("  [0] Cancel\n\n");

    int mode = 0;
    if (!ui_read_int("Select method", &mode, 0, 3) || mode == 0) {
        return;
    }

    if (mode == 1) {
        int order[MAX_MOVIES];
        int count = graph_bfs(&g_graph, seed_idx, order, MAX_MOVIES);
        ui_display_traversal_results("Breadth-First Search (BFS)", order, count, &g_movies);
    } else if (mode == 2) {
        int order[MAX_MOVIES];
        int count = graph_dfs(&g_graph, seed_idx, order, MAX_MOVIES);
        ui_display_traversal_results("Depth-First Search (DFS)", order, count, &g_movies);
    } else if (mode == 3) {
        printf("\nFetching live TMDB recommendations for \"%s\"...\n", seed->title);
        int http_code = 0;
        TmdbResult res = tmdb_fetch_recommendations(seed->id, 1, g_api_buffer, sizeof(g_api_buffer), &http_code);
        if (res != TMDB_SUCCESS) {
            printf("%s[Error]%s %s (HTTP: %d)\n", ANSI_RED, ANSI_RESET, tmdb_result_to_string(res), http_code);
        } else {
            MovieList rec_list;
            if (json_parse_movie_list(g_api_buffer, &rec_list) == JSON_PARSE_SUCCESS && rec_list.count > 0) {
                printf("\n%s=== TMDB Official Recommendations for \"%s\" ===%s\n",
                       ANSI_BOLD, seed->title, ANSI_RESET);
                ui_display_movie_table(&rec_list);
            } else {
                printf("%sNo specific recommendations returned by TMDB for this movie.%s\n",
                       ANSI_YELLOW, ANSI_RESET);
            }
        }
    }

    ui_pause_for_user();
}

static void handle_hash_table_lookup(void) {
    ui_clear_screen();
    ui_print_header("Hash Table Lookup by TMDB ID");

    if (g_movies.count == 0) {
        printf("\n%s[Notice]%s Hash table is currently empty. Load movies first.\n",
               ANSI_YELLOW, ANSI_RESET);
        ui_pause_for_user();
        return;
    }

    printf("Hash Table Status: %d active entries across %d buckets.\n",
           g_hash_table.size, HASH_TABLE_BUCKETS);
    printf("Currently loaded TMDB IDs for quick reference:\n  ");
    for (int i = 0; i < g_movies.count; i++) {
        printf("%d%s", g_movies.items[i].id, (i < g_movies.count - 1) ? ", " : "\n\n");
    }

    int target_id = 0;
    if (!ui_read_int("Enter TMDB ID to lookup in O(1) time", &target_id, 1, 9999999)) {
        return;
    }

    int found_idx = hash_table_find(&g_hash_table, target_id);
    if (found_idx >= 0 && found_idx < g_movies.count) {
        printf("\n%s[FOUND in Hash Table]%s Key %d mapped to array index %d!\n",
               ANSI_GREEN, ANSI_RESET, target_id, found_idx);
        ui_display_movie_card(&g_movies.items[found_idx]);
    } else {
        printf("\n%s[NOT FOUND]%s TMDB ID %d is not in the currently loaded cache.\n",
               ANSI_RED, ANSI_RESET, target_id);
    }

    ui_pause_for_user();
}

int main(int argc, char *argv[]) {
    /* Check for --no-color flag */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--no-color") == 0) {
            ui_set_color_enabled(false);
        }
    }

    if (!tmdb_global_init()) {
        fprintf(stderr, "[Fatal] Failed to initialize libcurl.\n");
        return 1;
    }

    movie_list_init(&g_movies);
    hash_table_init(&g_hash_table);
    graph_init(&g_graph, 0);

    /* Check credential setup */
    if (!tmdb_has_token()) {
        ui_clear_screen();
        ui_print_header("Authentication Setup Required");
        printf("\n%s[Warning] TMDB_BEARER_TOKEN is not set in your environment.%s\n\n",
               ANSI_RED, ANSI_RESET);
        printf("To query the live TMDB API, obtain your API Read Access Token from:\n");
        printf("  %shttps://www.themoviedb.org/settings/api%s\n\n", ANSI_CYAN, ANSI_RESET);
        printf("Then configure the environment variable in your terminal:\n");
        printf("  %sPowerShell:%s  $env:TMDB_BEARER_TOKEN = \"<YOUR_TOKEN>\"\n", ANSI_BOLD, ANSI_RESET);
        printf("  %sWindows CMD:%s set TMDB_BEARER_TOKEN=<YOUR_TOKEN>\n\n", ANSI_BOLD, ANSI_RESET);
        printf("The application can still demonstrate offline data structures or verify connectivity.\n");
        ui_print_footer();
        ui_print_attribution();
        ui_pause_for_user();
    }

    int choice = -1;
    while (choice != 0) {
        ui_clear_screen();
        ui_print_header("Movie Discovery & Recommendations");

        if (g_movies.count > 0) {
            ui_display_movie_table(&g_movies);
        } else {
            printf("\n%sTip: Start by discovering Popular movies [1] or searching [2].%s\n",
                   ANSI_GRAY, ANSI_RESET);
        }

        ui_print_main_menu();

        if (!ui_read_int("Select Menu Option", &choice, 0, 8)) {
            break;
        }

        switch (choice) {
            case 1:
                handle_discover_popular(1);
                break;
            case 2:
                handle_search(1);
                break;
            case 3:
                handle_filter_genre_lang();
                break;
            case 4:
                handle_sort();
                break;
            case 5:
                handle_view_details();
                break;
            case 6:
                handle_recommendations_graph();
                break;
            case 7:
                handle_hash_table_lookup();
                break;
            case 8:
                ui_print_about();
                break;
            case 0:
                printf("\nExiting CineMatch. Goodbye!\n");
                break;
            default:
                break;
        }
    }

    /* Clean up dynamic resources */
    hash_table_clear(&g_hash_table);
    graph_free(&g_graph);
    tmdb_global_cleanup();

    return 0;
}
