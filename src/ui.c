#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

static bool s_color_enabled = true;

void ui_set_color_enabled(bool enabled) {
    s_color_enabled = enabled;
}

bool ui_is_color_enabled(void) {
    return s_color_enabled;
}

/* Enable Windows 10/11 ANSI VT Processing */
static void enable_windows_vt_mode(void) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= 0x0004; /* ENABLE_VIRTUAL_TERMINAL_PROCESSING */
        SetConsoleMode(hOut, dwMode);
    }
#endif
}

void ui_clear_screen(void) {
    enable_windows_vt_mode();
    if (s_color_enabled) {
        printf("\x1b[2J\x1b[H");
    } else {
        printf("\n\n------------------------------------------------------------\n");
    }
}

static const char *c_bold(void) { return s_color_enabled ? ANSI_BOLD : ""; }
static const char *c_red(void) { return s_color_enabled ? ANSI_RED : ""; }
static const char *c_green(void) { return s_color_enabled ? ANSI_GREEN : ""; }
static const char *c_yellow(void) { return s_color_enabled ? ANSI_YELLOW : ""; }
static const char *c_cyan(void) { return s_color_enabled ? ANSI_CYAN : ""; }
static const char *c_gray(void) { return s_color_enabled ? ANSI_GRAY : ""; }
static const char *c_reset(void) { return s_color_enabled ? ANSI_RESET : ""; }

void ui_print_header(const char *subtitle) {
    enable_windows_vt_mode();
    printf("\n%s========================================================================%s\n", c_red(), c_reset());
    printf("%s%s                       C I N E M A T C H%s\n", c_bold(), c_red(), c_reset());
    printf("         %sDiscover Movies. Explore Graphs. Find Your Next Watch.%s\n", c_gray(), c_reset());
    if (subtitle && subtitle[0] != '\0') {
        printf("         %s>>> %s%s%s\n", c_yellow(), c_bold(), subtitle, c_reset());
    }
    printf("%s========================================================================%s\n", c_red(), c_reset());
}

void ui_print_footer(void) {
    printf("%s------------------------------------------------------------------------%s\n", c_gray(), c_reset());
}

void ui_print_attribution(void) {
    printf("%s[TMDB Attribution] This product uses the TMDB API but is not endorsed or certified by TMDB.%s\n",
           c_gray(), c_reset());
}

void ui_print_main_menu(void) {
    printf("\n%sMain Menu:%s\n", c_bold(), c_reset());
    printf("  %s[1]%s Discover Popular Movies      %s[5]%s View Movie Details Card\n", c_cyan(), c_reset(), c_cyan(), c_reset());
    printf("  %s[2]%s Search Movies by Title       %s[6]%s Graph Recommendations (BFS/DFS)\n", c_cyan(), c_reset(), c_cyan(), c_reset());
    printf("  %s[3]%s Filter by Genre / Language   %s[7]%s Hash Table Lookup by TMDB ID\n", c_cyan(), c_reset(), c_cyan(), c_reset());
    printf("  %s[4]%s Sort Loaded Results          %s[8]%s About & Data Structures\n", c_cyan(), c_reset(), c_cyan(), c_reset());
    printf("  %s[0]%s Exit Application\n", c_red(), c_reset());
    ui_print_footer();
    ui_print_attribution();
}

void ui_print_about(void) {
    ui_clear_screen();
    ui_print_header("About CineMatch & Data Structures Design");
    printf("\n%sCineMatch v1.0 — C11 Movie Discovery & Recommendation System%s\n\n", c_bold(), c_reset());
    printf("Developed for 3rd-Semester Data Structures coursework.\n\n");
    printf("%sImplemented Data Structures & Algorithms:%s\n", c_yellow(), c_reset());
    printf("  1. %sArray of Records%s: Fixed/dynamic cache storing fetched movie models.\n", c_bold(), c_reset());
    printf("  2. %sHash Table%s: O(1) average lookup by TMDB ID using separate chaining\n", c_bold(), c_reset());
    printf("     with %d buckets to resolve collisions.\n", HASH_TABLE_BUCKETS);
    printf("  3. %sGraph (Adjacency List)%s: Undirected graph over loaded movies where\n", c_bold(), c_reset());
    printf("     edges represent shared genres (weighted) or recommendation links.\n");
    printf("  4. %sBreadth-First Search (BFS)%s: Level-order traversal using a FIFO queue\n", c_bold(), c_reset());
    printf("     to discover closest genre-overlap neighbor movies.\n");
    printf("  5. %sDepth-First Search (DFS)%s: Recursive depth exploration to discover\n", c_bold(), c_reset());
    printf("     associative recommendation paths.\n");
    printf("  6. %sSorting Algorithms%s:\n", c_bold(), c_reset());
    printf("     - Insertion Sort (O(n^2)) for sorting movies by rating descending.\n");
    printf("  7. %sLinear Search & Substring Filtering%s: Case-insensitive query filter.\n\n",
           c_bold(), c_reset());
    printf("%sOfficial TMDB API Attribution:%s\n", c_yellow(), c_reset());
    printf("  \"This product uses the TMDB API but is not endorsed or certified by TMDB.\"\n");
    printf("  Base URL: https://api.themoviedb.org/3\n\n");
    ui_print_footer();
    ui_pause_for_user();
}

static void truncate_string(const char *src, char *dest, size_t dest_size, size_t max_visible) {
    if (!src || !dest || dest_size == 0) return;
    size_t len = strlen(src);
    if (len <= max_visible) {
        snprintf(dest, dest_size, "%s", src);
    } else {
        size_t keep = (max_visible > 3) ? max_visible - 3 : 0;
        snprintf(dest, dest_size, "%.*s...", (int)keep, src);
    }
}

void ui_display_movie_table(const MovieList *list) {
    if (!list || list->count == 0) {
        printf("\n%s[Notice] No movie records loaded in current view.%s\n", c_yellow(), c_reset());
        return;
    }

    printf("\n%s%-3s %-8s %-30s %-6s %-6s %-7s %-20s%s\n",
           c_bold(), "#", "ID", "TITLE", "YEAR", "LANG", "RATING", "GENRES", c_reset());
    ui_print_footer();

    for (int i = 0; i < list->count; i++) {
        const Movie *m = &list->items[i];

        char short_title[32];
        truncate_string(m->title, short_title, sizeof(short_title), 29);

        char year[8] = "N/A";
        if (strlen(m->release_date) >= 4) {
            snprintf(year, sizeof(year), "%.4s", m->release_date);
        }

        char genre_buf[32];
        movie_format_genres(m, genre_buf, sizeof(genre_buf));
        char short_genres[24];
        truncate_string(genre_buf, short_genres, sizeof(short_genres), 20);

        const char *rating_color = (m->rating >= 7.5) ? c_green() : (m->rating >= 5.0 ? c_yellow() : c_red());
        printf("%-3d %-8d %-30s %-6s %-6s %s%4.1f%s   %-20s\n",
               i + 1,
               m->id,
               short_title,
               year,
               m->original_language,
               rating_color, m->rating, c_reset(),
               short_genres);
    }
    ui_print_footer();
    printf("%sTotal Movies: %d | Page %d of %d (from TMDB)%s\n",
           c_gray(), list->count, list->page, list->total_pages, c_reset());
}

void ui_display_movie_card(const Movie *m) {
    if (!m) return;

    printf("\n%s------------------------------------------------------------------------%s\n", c_cyan(), c_reset());
    printf("%s%s [%d]%s\n", c_bold(), m->title, m->id, c_reset());
    printf("%s------------------------------------------------------------------------%s\n", c_cyan(), c_reset());
    const char *rating_color = (m->rating >= 7.5) ? c_green() : (m->rating >= 5.0 ? c_yellow() : c_red());
    printf("  %sRelease Date:%s %s  |  %sLanguage:%s %s  |  %sRating:%s %s%.1f / 10%s\n",
           c_bold(), c_reset(), m->release_date,
           c_bold(), c_reset(), m->original_language,
           c_bold(), c_reset(), rating_color, m->rating, c_reset());

    char genre_str[128];
    movie_format_genres(m, genre_str, sizeof(genre_str));
    printf("  %sGenres:%s       %s\n", c_bold(), c_reset(), genre_str);

    if (strcmp(m->poster_path, "N/A") != 0 && m->poster_path[0] != '\0') {
        printf("  %sPoster URL:%s   https://image.tmdb.org/t/p/w500%s\n",
               c_bold(), c_reset(), m->poster_path);
    } else {
        printf("  %sPoster URL:%s   N/A\n", c_bold(), c_reset());
    }

    printf("\n  %sOverview:%s\n", c_bold(), c_reset());

    /* Wrap overview text gracefully */
    const char *p = m->overview;
    while (*p) {
        while (*p == ' ') p++;
        if (!*p) break;
        char line[72];
        size_t len = 0;
        while (*p && len < 68) {
            line[len++] = *p++;
        }
        if (*p && *p != ' ') {
            /* backtrack to word boundary */
            size_t backtrack = len;
            while (backtrack > 0 && line[backtrack - 1] != ' ') {
                backtrack--;
            }
            if (backtrack > 20) {
                p -= (len - backtrack);
                len = backtrack;
            }
        }
        line[len] = '\0';
        printf("    %s\n", line);
    }
    printf("%s------------------------------------------------------------------------%s\n", c_cyan(), c_reset());
}

void ui_display_graph_structure(const Graph *g, const MovieList *list) {
    if (!g || !list || g->num_vertices == 0) {
        printf("%s[Notice] Graph is empty.%s\n", c_yellow(), c_reset());
        return;
    }

    printf("\n%s=== Local Movie Similarity Graph (Adjacency List) ===%s\n", c_bold(), c_reset());
    printf("%sVertices: %d movies | Edge weights = Number of shared genres%s\n\n",
           c_gray(), g->num_vertices, c_reset());

    for (int i = 0; i < g->num_vertices && i < list->count; i++) {
        char title_i[24];
        truncate_string(list->items[i].title, title_i, sizeof(title_i), 22);

        printf("  [%d] %-22s -> ", i + 1, title_i);

        GraphEdgeNode *curr = g->adj[i].head;
        if (!curr) {
            printf("%s(isolated node)%s\n", c_gray(), c_reset());
            continue;
        }

        while (curr) {
            int dest = curr->dest;
            if (dest >= 0 && dest < list->count) {
                char title_dest[18];
                truncate_string(list->items[dest].title, title_dest, sizeof(title_dest), 16);
                printf("[%d: %s (w:%d)] ", dest + 1, title_dest, curr->weight);
            }
            curr = curr->next;
        }
        printf("\n");
    }
    ui_print_footer();
}

void ui_display_traversal_results(const char *algo_name, const int *indices, int count, const MovieList *list) {
    if (!algo_name || !indices || !list || count == 0) {
        printf("%s[Notice] No traversal sequence generated.%s\n", c_yellow(), c_reset());
        return;
    }

    printf("\n%s=== %s Traversal Order ===%s\n", c_bold(), algo_name, c_reset());
    printf("%sVisited sequence of related movies:%s\n\n", c_yellow(), c_reset());

    for (int i = 0; i < count; i++) {
        int idx = indices[i];
        if (idx >= 0 && idx < list->count) {
            const Movie *m = &list->items[idx];
            char genres[32];
            movie_format_genres(m, genres, sizeof(genres));

            printf("  Step %d: [%d] %s%s%s (Rating: %.1f, Genres: %s)\n",
                   i + 1, idx + 1, c_bold(), m->title, c_reset(), m->rating, genres);
        }
    }
    ui_print_footer();
}

bool ui_read_string(const char *prompt, char *buffer, size_t buf_size) {
    if (!prompt || !buffer || buf_size == 0) return false;
    buffer[0] = '\0';

    printf("%s%s%s", c_yellow(), prompt, c_reset());
    fflush(stdout);

    if (!fgets(buffer, (int)buf_size, stdin)) {
        return false;
    }

    /* Trim trailing newline and carriage returns */
    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
        buffer[len - 1] = '\0';
        len--;
    }

    /* Trim leading whitespace */
    char *start = buffer;
    while (*start == ' ' || *start == '\t') start++;
    if (start != buffer) {
        memmove(buffer, start, strlen(start) + 1);
    }

    return (buffer[0] != '\0');
}

bool ui_read_int(const char *prompt, int *value_out, int min_val, int max_val) {
    if (!prompt || !value_out) return false;

    char line[64];
    while (1) {
        printf("%s%s (%d - %d): %s", c_yellow(), prompt, min_val, max_val, c_reset());
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            return false;
        }

        char *endptr = NULL;
        long val = strtol(line, &endptr, 10);
        while (*endptr == ' ' || *endptr == '\t' || *endptr == '\r' || *endptr == '\n') {
            endptr++;
        }

        if (endptr != line && *endptr == '\0') {
            if (val >= min_val && val <= max_val) {
                *value_out = (int)val;
                return true;
            }
        }
        printf("%sInvalid entry. Please enter an integer between %d and %d.%s\n",
               c_red(), min_val, max_val, c_reset());
    }
}

void ui_pause_for_user(void) {
    char dummy[16];
    printf("\n%sPress [Enter] to continue...%s", c_gray(), c_reset());
    fflush(stdout);
    if (!fgets(dummy, sizeof(dummy), stdin)) {
        /* EOF reached */
    }
}
