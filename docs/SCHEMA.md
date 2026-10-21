# Data Schema
**Status:** LOCKED v1.0

## Movie record (C)
```c
typedef struct {
    int id;                         // TMDB ID
    char title[160];
    char original_language[8];      // ISO 639-1, e.g. hi, en, ta
    char release_date[11];          // YYYY-MM-DD or empty
    double rating;                  // TMDB vote_average
    int genre_ids[8];               // TMDB genre IDs
    int genre_count;
    char overview[512];
    char poster_path[256];          // path, not full URL
} Movie;
```

## API JSON fields mapped
- `id` -> `Movie.id`
- `title` -> `Movie.title`
- `original_language` -> `Movie.original_language`
- `release_date` -> `Movie.release_date`
- `vote_average` -> `Movie.rating`
- `genre_ids[]` -> `Movie.genre_ids[]`
- `overview` -> `Movie.overview`
- `poster_path` -> `Movie.poster_path`

## Runtime collections
- `Movie results[MAX_MOVIES]`: only current fetched page/results.
- `HashEntry table[HASH_SIZE]`: movie ID -> index in results (if hash table milestone completed).
- `int head[MAX_MOVIES]` + `Edge edges[MAX_EDGES]` (or small per-node linked adjacency lists): local similarity graph.

## Graph semantics
Nodes are movie records currently in memory. Add an undirected edge if two loaded movies share at least one genre (simple mode), or if one is returned as a similar/recommended movie (API mode). State clearly which rule is implemented. Do not imply this graph contains all movies in TMDB.
