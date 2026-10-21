#include "movie.h"
#include <string.h>
#include <stdio.h>

/* Official TMDB Standard Movie Genres */
static const GenreInfo TMDB_GENRES[] = {
    {28, "Action"},
    {12, "Adventure"},
    {16, "Animation"},
    {35, "Comedy"},
    {80, "Crime"},
    {99, "Documentary"},
    {18, "Drama"},
    {10751, "Family"},
    {14, "Fantasy"},
    {36, "History"},
    {27, "Horror"},
    {10402, "Music"},
    {9648, "Mystery"},
    {10749, "Romance"},
    {878, "Sci-Fi"},
    {10770, "TV Movie"},
    {53, "Thriller"},
    {10752, "War"},
    {37, "Western"}
};

static const size_t TMDB_GENRE_COUNT = sizeof(TMDB_GENRES) / sizeof(TMDB_GENRES[0]);

void movie_init(Movie *m) {
    if (!m) return;
    m->id = 0;
    m->title[0] = '\0';
    strcpy(m->original_language, "N/A");
    strcpy(m->release_date, "N/A");
    m->rating = 0.0;
    m->genre_count = 0;
    for (int i = 0; i < MAX_GENRES_PER_MOVIE; i++) {
        m->genre_ids[i] = 0;
    }
    strcpy(m->overview, "No overview available.");
    strcpy(m->poster_path, "N/A");
}

void movie_list_init(MovieList *list) {
    if (!list) return;
    list->count = 0;
    list->page = 1;
    list->total_pages = 1;
    list->total_results = 0;
    for (int i = 0; i < MAX_MOVIES; i++) {
        movie_init(&list->items[i]);
    }
}

const char *movie_get_genre_name(int genre_id) {
    for (size_t i = 0; i < TMDB_GENRE_COUNT; i++) {
        if (TMDB_GENRES[i].id == genre_id) {
            return TMDB_GENRES[i].name;
        }
    }
    return "Other";
}

void movie_format_genres(const Movie *m, char *buffer, size_t buf_size) {
    if (!m || !buffer || buf_size == 0) return;
    buffer[0] = '\0';

    if (m->genre_count == 0) {
        snprintf(buffer, buf_size, "N/A");
        return;
    }

    size_t offset = 0;
    for (int i = 0; i < m->genre_count && i < MAX_GENRES_PER_MOVIE; i++) {
        const char *name = movie_get_genre_name(m->genre_ids[i]);
        int written = snprintf(buffer + offset, buf_size - offset, "%s%s",
                               (i > 0) ? ", " : "", name);
        if (written < 0 || (size_t)written >= buf_size - offset) {
            break;
        }
        offset += (size_t)written;
    }
}
