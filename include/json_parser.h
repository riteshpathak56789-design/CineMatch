#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "movie.h"
#include <stdbool.h>
#include <stddef.h>

#define MAX_JSON_TOKENS 4096

typedef enum {
    JSON_PARSE_SUCCESS = 0,
    JSON_PARSE_ERR_INVALID,
    JSON_PARSE_ERR_NO_MEMORY,
    JSON_PARSE_ERR_UNEXPECTED_STRUCTURE,
    JSON_PARSE_ERR_EMPTY
} JsonParseResult;

/* Parse a paginated TMDB response (e.g. popular, search, discover, recommendations) into MovieList */
JsonParseResult json_parse_movie_list(const char *json_str, MovieList *list);

/* Parse a single movie details response into Movie struct */
JsonParseResult json_parse_single_movie(const char *json_str, Movie *movie);

/* Parse TMDB error message if present (e.g. status_message) */
bool json_parse_tmdb_error(const char *json_str, char *msg_out, size_t msg_max);

#endif /* JSON_PARSER_H */
