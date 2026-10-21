#ifndef TMDB_H
#define TMDB_H

#include <stdbool.h>
#include <stddef.h>

#define TMDB_BASE_URL "https://api.themoviedb.org/3"
#define TMDB_ENV_VAR "TMDB_BEARER_TOKEN"
#define TMDB_MAX_RESPONSE_SIZE (256 * 1024) /* 256 KB response buffer cap */
#define TMDB_TIMEOUT_SECONDS 12

typedef enum {
    TMDB_SUCCESS = 0,
    TMDB_ERR_NO_TOKEN,
    TMDB_ERR_NETWORK,
    TMDB_ERR_HTTP_STATUS,
    TMDB_ERR_BUFFER_OVERFLOW,
    TMDB_ERR_INVALID_PARAM,
    TMDB_ERR_UNAUTHORIZED,
    TMDB_ERR_NOT_FOUND,
    TMDB_ERR_RATE_LIMIT
} TmdbResult;

/* Global lifecycle */
bool tmdb_global_init(void);
void tmdb_global_cleanup(void);

/* Credential check (never prints the secret) */
bool tmdb_has_token(void);

/* Convert error code to friendly message */
const char *tmdb_result_to_string(TmdbResult res);

/* URL encoder helper */
bool tmdb_url_encode(const char *input, char *output, size_t out_size);

/* Low-level HTTP GET request handler */
TmdbResult tmdb_http_get(const char *url, bool require_auth, char *response_buf,
                         size_t max_buf_size, int *http_code);

/* High-level TMDB API endpoints */
TmdbResult tmdb_fetch_popular(int page, char *buf, size_t buf_size, int *http_code);
TmdbResult tmdb_fetch_search(const char *query, int page, char *buf, size_t buf_size, int *http_code);
TmdbResult tmdb_fetch_discover(int genre_id, const char *lang_iso, int page, char *buf, size_t buf_size, int *http_code);
TmdbResult tmdb_fetch_details(int movie_id, char *buf, size_t buf_size, int *http_code);
TmdbResult tmdb_fetch_recommendations(int movie_id, int page, char *buf, size_t buf_size, int *http_code);

/* Test connectivity check */
TmdbResult tmdb_ping_unauthenticated(int *http_code);

#endif /* TMDB_H */
