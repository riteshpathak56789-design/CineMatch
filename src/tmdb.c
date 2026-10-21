#include "tmdb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct MemoryBuffer {
    char *data;
    size_t size;
    size_t max_size;
    bool overflow;
};

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    struct MemoryBuffer *mem = (struct MemoryBuffer *)userp;

    if (mem->size + real_size >= mem->max_size) {
        mem->overflow = true;
        return 0; /* Returning 0 causes curl_easy_perform to fail with CURLE_WRITE_ERROR */
    }

    memcpy(mem->data + mem->size, contents, real_size);
    mem->size += real_size;
    mem->data[mem->size] = '\0';
    return real_size;
}

bool tmdb_global_init(void) {
    CURLcode code = curl_global_init(CURL_GLOBAL_DEFAULT);
    return (code == CURLE_OK);
}

void tmdb_global_cleanup(void) {
    curl_global_cleanup();
}

bool tmdb_has_token(void) {
    const char *token = getenv(TMDB_ENV_VAR);
    return (token != NULL && token[0] != '\0');
}

const char *tmdb_result_to_string(TmdbResult res) {
    switch (res) {
        case TMDB_SUCCESS:
            return "Operation successful.";
        case TMDB_ERR_NO_TOKEN:
            return "TMDB_BEARER_TOKEN is not set in environment. Set your TMDB API read access token.";
        case TMDB_ERR_UNAUTHORIZED:
            return "Authentication failed (HTTP 401/403). Please verify your TMDB API bearer token.";
        case TMDB_ERR_NOT_FOUND:
            return "Resource not found on TMDB (HTTP 404).";
        case TMDB_ERR_RATE_LIMIT:
            return "TMDB API rate limit exceeded (HTTP 429). Please wait and retry.";
        case TMDB_ERR_HTTP_STATUS:
            return "TMDB returned an unexpected HTTP error status.";
        case TMDB_ERR_NETWORK:
            return "Network communication failure or connection timeout.";
        case TMDB_ERR_BUFFER_OVERFLOW:
            return "Response size exceeded buffer allocation limit.";
        case TMDB_ERR_INVALID_PARAM:
            return "Invalid request parameter or query provided.";
        default:
            return "Unknown error.";
    }
}

bool tmdb_url_encode(const char *input, char *output, size_t out_size) {
    if (!input || !output || out_size == 0) return false;
    output[0] = '\0';

    CURL *curl = curl_easy_init();
    if (!curl) return false;

    char *escaped = curl_easy_escape(curl, input, (int)strlen(input));
    if (!escaped) {
        curl_easy_cleanup(curl);
        return false;
    }

    size_t esc_len = strlen(escaped);
    if (esc_len >= out_size) {
        curl_free(escaped);
        curl_easy_cleanup(curl);
        return false;
    }

    strncpy(output, escaped, out_size);
    output[out_size - 1] = '\0';

    curl_free(escaped);
    curl_easy_cleanup(curl);
    return true;
}

TmdbResult tmdb_http_get(const char *url, bool require_auth, char *response_buf,
                         size_t max_buf_size, int *http_code) {
    if (!url || !response_buf || max_buf_size == 0) {
        return TMDB_ERR_INVALID_PARAM;
    }

    response_buf[0] = '\0';
    if (http_code) *http_code = 0;

    const char *token = getenv(TMDB_ENV_VAR);
    if (require_auth && (!token || token[0] == '\0')) {
        return TMDB_ERR_NO_TOKEN;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        return TMDB_ERR_NETWORK;
    }

    struct MemoryBuffer chunk;
    chunk.data = response_buf;
    chunk.size = 0;
    chunk.max_size = max_buf_size;
    chunk.overflow = false;

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "accept: application/json");

    if (require_auth && token) {
        /* Allocate header buffer safely without logging */
        size_t token_len = strlen(token);
        size_t header_len = token_len + 32;
        char *auth_hdr = (char *)malloc(header_len);
        if (auth_hdr) {
            snprintf(auth_hdr, header_len, "Authorization: Bearer %s", token);
            headers = curl_slist_append(headers, auth_hdr);
            /* Scrub memory */
            memset(auth_hdr, 0, header_len);
            free(auth_hdr);
        }
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "CineMatch/1.0");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)TMDB_TIMEOUT_SECONDS);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 8L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res = curl_easy_perform(curl);

    long code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    if (http_code) *http_code = (int)code;

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (chunk.overflow) {
        return TMDB_ERR_BUFFER_OVERFLOW;
    }

    if (res != CURLE_OK) {
        return TMDB_ERR_NETWORK;
    }

    if (code == 401 || code == 403) {
        return TMDB_ERR_UNAUTHORIZED;
    }
    if (code == 404) {
        return TMDB_ERR_NOT_FOUND;
    }
    if (code == 429) {
        return TMDB_ERR_RATE_LIMIT;
    }
    if (code < 200 || code >= 300) {
        return TMDB_ERR_HTTP_STATUS;
    }

    return TMDB_SUCCESS;
}

TmdbResult tmdb_fetch_popular(int page, char *buf, size_t buf_size, int *http_code) {
    if (page < 1) page = 1;
    char url[256];
    snprintf(url, sizeof(url), "%s/movie/popular?language=en-US&page=%d", TMDB_BASE_URL, page);
    return tmdb_http_get(url, true, buf, buf_size, http_code);
}

TmdbResult tmdb_fetch_search(const char *query, int page, char *buf, size_t buf_size, int *http_code) {
    if (!query || query[0] == '\0') return TMDB_ERR_INVALID_PARAM;
    if (page < 1) page = 1;

    char encoded_query[256];
    if (!tmdb_url_encode(query, encoded_query, sizeof(encoded_query))) {
        return TMDB_ERR_INVALID_PARAM;
    }

    char url[512];
    snprintf(url, sizeof(url), "%s/search/movie?query=%s&include_adult=false&language=en-US&page=%d",
             TMDB_BASE_URL, encoded_query, page);
    return tmdb_http_get(url, true, buf, buf_size, http_code);
}

TmdbResult tmdb_fetch_discover(int genre_id, const char *lang_iso, int page, char *buf, size_t buf_size, int *http_code) {
    if (page < 1) page = 1;

    char url[512];
    int offset = snprintf(url, sizeof(url), "%s/discover/movie?include_adult=false&sort_by=vote_average.desc&vote_count.gte=100&page=%d",
                          TMDB_BASE_URL, page);

    if (genre_id > 0 && offset < (int)sizeof(url)) {
        offset += snprintf(url + offset, sizeof(url) - offset, "&with_genres=%d", genre_id);
    }

    if (lang_iso && lang_iso[0] != '\0' && offset < (int)sizeof(url)) {
        char enc_lang[32];
        if (tmdb_url_encode(lang_iso, enc_lang, sizeof(enc_lang))) {
            snprintf(url + offset, sizeof(url) - offset, "&with_original_language=%s", enc_lang);
        }
    }

    return tmdb_http_get(url, true, buf, buf_size, http_code);
}

TmdbResult tmdb_fetch_details(int movie_id, char *buf, size_t buf_size, int *http_code) {
    if (movie_id <= 0) return TMDB_ERR_INVALID_PARAM;
    char url[256];
    snprintf(url, sizeof(url), "%s/movie/%d?language=en-US", TMDB_BASE_URL, movie_id);
    return tmdb_http_get(url, true, buf, buf_size, http_code);
}

TmdbResult tmdb_fetch_recommendations(int movie_id, int page, char *buf, size_t buf_size, int *http_code) {
    if (movie_id <= 0) return TMDB_ERR_INVALID_PARAM;
    if (page < 1) page = 1;
    char url[256];
    snprintf(url, sizeof(url), "%s/movie/%d/recommendations?language=en-US&page=%d",
             TMDB_BASE_URL, movie_id, page);
    return tmdb_http_get(url, true, buf, buf_size, http_code);
}

TmdbResult tmdb_ping_unauthenticated(int *http_code) {
    char url[256];
    snprintf(url, sizeof(url), "%s/configuration", TMDB_BASE_URL);
    char buf[1024];
    /* Unauthenticated call to check network reachability */
    return tmdb_http_get(url, false, buf, sizeof(buf), http_code);
}
