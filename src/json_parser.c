#include "json_parser.h"
#include "jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Helper to compare a JSON token with a string */
static bool jsoneq(const char *json, const jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING &&
        (int)strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return true;
    }
    return false;
}

/* Helper to copy string token content with bounds checking */
static bool json_copy_string(const char *json, const jsmntok_t *tok, char *dest, size_t dest_size) {
    if (!json || !tok || !dest || dest_size == 0) return false;
    dest[0] = '\0';

    if (tok->type == JSMN_PRIMITIVE && strncmp(json + tok->start, "null", 4) == 0) {
        snprintf(dest, dest_size, "N/A");
        return true;
    }

    if (tok->type != JSMN_STRING) return false;

    int len = tok->end - tok->start;
    if (len <= 0) {
        snprintf(dest, dest_size, "N/A");
        return true;
    }

    size_t copy_len = ((size_t)len < dest_size - 1) ? (size_t)len : dest_size - 1;
    memcpy(dest, json + tok->start, copy_len);
    dest[copy_len] = '\0';
    return true;
}

/* Helper to parse an integer from a primitive token */
static int json_parse_int(const char *json, const jsmntok_t *tok, int default_val) {
    if (!json || !tok) return default_val;
    if (tok->type != JSMN_PRIMITIVE) return default_val;

    char temp[32];
    int len = tok->end - tok->start;
    if (len <= 0 || len >= (int)sizeof(temp)) return default_val;

    memcpy(temp, json + tok->start, len);
    temp[len] = '\0';
    return atoi(temp);
}

/* Helper to parse a double from a primitive token */
static double json_parse_double(const char *json, const jsmntok_t *tok, double default_val) {
    if (!json || !tok) return default_val;
    if (tok->type != JSMN_PRIMITIVE) return default_val;

    char temp[32];
    int len = tok->end - tok->start;
    if (len <= 0 || len >= (int)sizeof(temp)) return default_val;

    memcpy(temp, json + tok->start, len);
    temp[len] = '\0';
    return atof(temp);
}

/* Helper to skip a token and all its nested children using token boundaries */
static int skip_token(const jsmntok_t *tokens, int index) {
    if (tokens[index].start < 0 || tokens[index].end < 0) {
        return index + 1;
    }
    int end = tokens[index].end;
    index++;
    while (tokens[index].start >= 0 && tokens[index].start < end) {
        index++;
    }
    return index;
}

/* Parse a single movie object into Movie struct */
static int parse_movie_object(const char *json, const jsmntok_t *tokens, int obj_idx, Movie *m) {
    movie_init(m);
    if (tokens[obj_idx].type != JSMN_OBJECT) {
        return skip_token(tokens, obj_idx);
    }

    int obj_end = tokens[obj_idx].end;
    int idx = obj_idx + 1;

    while (tokens[idx].start >= 0 && tokens[idx].start < obj_end) {
        const jsmntok_t *key_tok = &tokens[idx];
        const jsmntok_t *val_tok = &tokens[idx + 1];

        if (jsoneq(json, key_tok, "id")) {
            m->id = json_parse_int(json, val_tok, 0);
            idx = skip_token(tokens, idx + 1);
        } else if (jsoneq(json, key_tok, "title")) {
            json_copy_string(json, val_tok, m->title, sizeof(m->title));
            idx = skip_token(tokens, idx + 1);
        } else if (jsoneq(json, key_tok, "original_language")) {
            json_copy_string(json, val_tok, m->original_language, sizeof(m->original_language));
            idx = skip_token(tokens, idx + 1);
        } else if (jsoneq(json, key_tok, "release_date")) {
            json_copy_string(json, val_tok, m->release_date, sizeof(m->release_date));
            idx = skip_token(tokens, idx + 1);
        } else if (jsoneq(json, key_tok, "vote_average")) {
            m->rating = json_parse_double(json, val_tok, 0.0);
            idx = skip_token(tokens, idx + 1);
        } else if (jsoneq(json, key_tok, "overview")) {
            json_copy_string(json, val_tok, m->overview, sizeof(m->overview));
            idx = skip_token(tokens, idx + 1);
        } else if (jsoneq(json, key_tok, "poster_path")) {
            json_copy_string(json, val_tok, m->poster_path, sizeof(m->poster_path));
            idx = skip_token(tokens, idx + 1);
        } else if (jsoneq(json, key_tok, "genre_ids")) {
            if (val_tok->type == JSMN_ARRAY) {
                int arr_end = val_tok->end;
                int g_idx = idx + 2;
                m->genre_count = 0;
                while (tokens[g_idx].start >= 0 && tokens[g_idx].start < arr_end) {
                    if (m->genre_count < MAX_GENRES_PER_MOVIE) {
                        m->genre_ids[m->genre_count++] = json_parse_int(json, &tokens[g_idx], 0);
                    }
                    g_idx = skip_token(tokens, g_idx);
                }
                idx = g_idx;
            } else {
                idx = skip_token(tokens, idx + 1);
            }
        } else if (jsoneq(json, key_tok, "genres")) {
            /* Handled for movie details where genres is array of {id, name} objects */
            if (val_tok->type == JSMN_ARRAY) {
                int arr_end = val_tok->end;
                int g_idx = idx + 2;
                m->genre_count = 0;
                while (tokens[g_idx].start >= 0 && tokens[g_idx].start < arr_end) {
                    if (tokens[g_idx].type == JSMN_OBJECT) {
                        int inner_end = tokens[g_idx].end;
                        int cur = g_idx + 1;
                        int genre_id = 0;
                        while (tokens[cur].start >= 0 && tokens[cur].start < inner_end) {
                            if (jsoneq(json, &tokens[cur], "id") && tokens[cur + 1].start < inner_end) {
                                genre_id = json_parse_int(json, &tokens[cur + 1], 0);
                            }
                            cur = skip_token(tokens, cur);
                        }
                        if (genre_id > 0 && m->genre_count < MAX_GENRES_PER_MOVIE) {
                            m->genre_ids[m->genre_count++] = genre_id;
                        }
                    }
                    g_idx = skip_token(tokens, g_idx);
                }
                idx = g_idx;
            } else {
                idx = skip_token(tokens, idx + 1);
            }
        } else {
            /* Skip key and its value */
            idx = skip_token(tokens, idx + 1);
        }
    }
    return idx;
}

JsonParseResult json_parse_movie_list(const char *json_str, MovieList *list) {
    if (!json_str || !list) return JSON_PARSE_ERR_INVALID;
    movie_list_init(list);

    size_t json_len = strlen(json_str);
    if (json_len == 0) return JSON_PARSE_ERR_EMPTY;

    jsmn_parser parser;
    jsmn_init(&parser);

    jsmntok_t *tokens = (jsmntok_t *)malloc(MAX_JSON_TOKENS * sizeof(jsmntok_t));
    if (!tokens) return JSON_PARSE_ERR_NO_MEMORY;

    int num_tokens = jsmn_parse(&parser, json_str, json_len, tokens, MAX_JSON_TOKENS);
    if (num_tokens < 0) {
        free(tokens);
        return JSON_PARSE_ERR_INVALID;
    }

    if (num_tokens < 1 || tokens[0].type != JSMN_OBJECT) {
        free(tokens);
        return JSON_PARSE_ERR_UNEXPECTED_STRUCTURE;
    }

    int idx = 1;
    int top_pairs = tokens[0].size;

    for (int p = 0; p < top_pairs && idx < num_tokens; p++) {
        const jsmntok_t *key_tok = &tokens[idx];
        const jsmntok_t *val_tok = &tokens[idx + 1];

        if (jsoneq(json_str, key_tok, "page")) {
            list->page = json_parse_int(json_str, val_tok, 1);
            idx += 2;
        } else if (jsoneq(json_str, key_tok, "total_pages")) {
            list->total_pages = json_parse_int(json_str, val_tok, 1);
            idx += 2;
        } else if (jsoneq(json_str, key_tok, "total_results")) {
            list->total_results = json_parse_int(json_str, val_tok, 0);
            idx += 2;
        } else if (jsoneq(json_str, key_tok, "results")) {
            if (val_tok->type == JSMN_ARRAY) {
                int array_len = val_tok->size;
                int item_idx = idx + 2;
                for (int i = 0; i < array_len && item_idx < num_tokens; i++) {
                    if (list->count < MAX_MOVIES) {
                        item_idx = parse_movie_object(json_str, tokens, item_idx, &list->items[list->count]);
                        list->count++;
                    } else {
                        item_idx = skip_token(tokens, item_idx);
                    }
                }
                idx = item_idx;
            } else {
                idx = skip_token(tokens, idx + 1);
            }
        } else {
            idx++;
            idx = skip_token(tokens, idx);
        }
    }

    free(tokens);
    return JSON_PARSE_SUCCESS;
}

JsonParseResult json_parse_single_movie(const char *json_str, Movie *movie) {
    if (!json_str || !movie) return JSON_PARSE_ERR_INVALID;
    movie_init(movie);

    size_t json_len = strlen(json_str);
    if (json_len == 0) return JSON_PARSE_ERR_EMPTY;

    jsmn_parser parser;
    jsmn_init(&parser);

    jsmntok_t *tokens = (jsmntok_t *)malloc(MAX_JSON_TOKENS * sizeof(jsmntok_t));
    if (!tokens) return JSON_PARSE_ERR_NO_MEMORY;

    int num_tokens = jsmn_parse(&parser, json_str, json_len, tokens, MAX_JSON_TOKENS);
    if (num_tokens < 0) {
        free(tokens);
        return JSON_PARSE_ERR_INVALID;
    }

    if (num_tokens < 1 || tokens[0].type != JSMN_OBJECT) {
        free(tokens);
        return JSON_PARSE_ERR_UNEXPECTED_STRUCTURE;
    }

    parse_movie_object(json_str, tokens, 0, movie);
    free(tokens);
    return JSON_PARSE_SUCCESS;
}

bool json_parse_tmdb_error(const char *json_str, char *msg_out, size_t msg_max) {
    if (!json_str || !msg_out || msg_max == 0) return false;
    msg_out[0] = '\0';

    size_t json_len = strlen(json_str);
    if (json_len == 0) return false;

    jsmn_parser parser;
    jsmn_init(&parser);

    jsmntok_t tokens[64];
    int num = jsmn_parse(&parser, json_str, json_len, tokens, 64);
    if (num <= 0 || tokens[0].type != JSMN_OBJECT) return false;

    for (int i = 1; i < num; i++) {
        if (jsoneq(json_str, &tokens[i], "status_message") && i + 1 < num) {
            return json_copy_string(json_str, &tokens[i + 1], msg_out, msg_max);
        }
    }
    return false;
}
