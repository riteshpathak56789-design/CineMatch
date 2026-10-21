#ifndef MOVIE_H
#define MOVIE_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_MOVIES 20
#define MAX_TITLE 160
#define MAX_OVERVIEW 512
#define MAX_GENRES_PER_MOVIE 8
#define MAX_POSTER_PATH 256
#define MAX_LANG_LEN 8
#define MAX_DATE_LEN 11

/**
 * Movie record representation as specified in SCHEMA.md
 */
typedef struct {
    int id;                                 /* TMDB Movie ID */
    char title[MAX_TITLE];                  /* Movie title */
    char original_language[MAX_LANG_LEN];   /* ISO 639-1 code (e.g. en, hi, ja) */
    char release_date[MAX_DATE_LEN];        /* YYYY-MM-DD or empty string */
    double rating;                          /* TMDB vote_average (0.0 to 10.0) */
    int genre_ids[MAX_GENRES_PER_MOVIE];    /* TMDB genre IDs */
    int genre_count;                        /* Number of valid genres in genre_ids */
    char overview[MAX_OVERVIEW];            /* Summary overview text */
    char poster_path[MAX_POSTER_PATH];      /* Poster path suffix (e.g. /xyz.jpg) */
} Movie;

/**
 * Movie collection container
 */
typedef struct {
    Movie items[MAX_MOVIES];
    int count;
    int page;
    int total_pages;
    int total_results;
} MovieList;

/**
 * Known TMDB Genre map entry
 */
typedef struct {
    int id;
    const char *name;
} GenreInfo;

/* Helper functions for Movie models */
void movie_init(Movie *m);
void movie_list_init(MovieList *list);
const char *movie_get_genre_name(int genre_id);
void movie_format_genres(const Movie *m, char *buffer, size_t buf_size);

#endif /* MOVIE_H */
