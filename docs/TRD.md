# TRD — Technical Requirements & Design
**Status:** LOCKED v1.0

## 1. Architecture
`CLI menu (C) -> input validation -> TMDB client (libcurl) -> JSON parser (jsmn) -> Movie[] -> DS operations -> terminal output`

No separate web server or frontend. All application logic is in C.

## 2. Runtime/dependencies
- C compiler: GCC or Clang, C11.
- HTTP: libcurl.
- JSON: small token-based parser such as jsmn; vendor its source with attribution/license intact.
- OS: Linux/WSL easiest; Windows can use MSYS2/MinGW with libcurl. Do not assume dependencies exist—verify first.

## 3. TMDB requests
Base URL: `https://api.themoviedb.org/3`
Auth: send `Authorization: Bearer <TMDB_BEARER_TOKEN>` header.
Endpoints:
- Search: `/search/movie?query=<url-encoded-query>&include_adult=false&page=1`
- Discover: `/discover/movie?with_genres=<genre_id>&with_original_language=<iso_code>&sort_by=vote_average.desc&page=1`
- Genre list: `/genre/movie/list`
- Details: `/movie/{movie_id}`
- Recommendations: `/movie/{movie_id}/recommendations?page=1`
- Similar movies (optional): `/movie/{movie_id}/similar?page=1`

Always URL-encode user-provided search text. Check HTTP status and libcurl errors. Do not print the bearer token to logs. Do not expose it in source code.

## 4. Data model
```c
#define MAX_MOVIES 20
#define MAX_TITLE 160
#define MAX_OVERVIEW 512
#define MAX_GENRES_PER_MOVIE 8

typedef struct {
    int id;
    char title[MAX_TITLE];
    char original_language[8];
    char release_date[11];
    double rating;
    int genre_ids[MAX_GENRES_PER_MOVIE];
    int genre_count;
    char overview[MAX_OVERVIEW];
    char poster_path[256];
} Movie;
```
For the first build, a compact `Movie movies[MAX_MOVIES]` is enough. Do not build a full offline mirror of TMDB.

## 5. Data Structures mapped to requirements
- Array of structures: current page/result cache.
- Hash table: optional lookup by TMDB movie ID, collision resolution by chaining or linear probing. If implementation time is tight, make this the next milestone, not a fake claim.
- Graph adjacency list: graph only over the small set of movies currently loaded, not the whole TMDB catalogue. Add an edge when movies share a genre or appear in the TMDB similar/recommendations response.
- BFS/DFS: traverse this local graph to show related results.
- Sorting: insertion sort or selection sort by rating; explain O(n²) is acceptable for small result sets.
- Filtering/search: linear scan is acceptable and easy to explain.

## 6. Error handling
- Missing `TMDB_BEARER_TOKEN`: explain how to set it, exit cleanly.
- HTTP != 200: show status and concise error, never the credential.
- Empty results: “No movies found; change your query/filter.”
- Null `poster_path`, `release_date`, overview: show `N/A`.
- JSON token/depth overflow: fail safely; increase fixed buffer only with a clear limit.
- Network timeout: set a finite timeout (e.g. 10–15 seconds).

## 7. Build
Example Linux/WSL:
`gcc -std=c11 -Wall -Wextra -pedantic src/main.c src/tmdb.c src/ds.c -lcurl -o cinematch`
Exact command may change depending on whether jsmn is compiled separately.

## 8. Security and compliance
- Never hardcode credentials.
- Keep `.env`, local config and build outputs out of Git.
- Do not scrape TMDB/BookMyShow pages; use official TMDB API.
- Include required TMDB attribution.
- Personal/non-commercial use must comply with current TMDB terms.
