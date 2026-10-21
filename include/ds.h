#ifndef DS_H
#define DS_H

#include "movie.h"
#include <stdbool.h>
#include <stddef.h>

/* ========================================================================= */
/* 1. HASH TABLE (TMDB ID -> Movie Index)                                   */
/* Separate chaining collision resolution                                    */
/* Time Complexity: Average O(1) Search/Insert, Worst O(n)                   */
/* ========================================================================= */
#define HASH_TABLE_BUCKETS 31

typedef struct HashNode {
    int movie_id;               /* Key: TMDB Movie ID */
    int movie_index;            /* Value: Index in MovieList results array */
    struct HashNode *next;      /* Collision chain pointer */
} HashNode;

typedef struct {
    HashNode *buckets[HASH_TABLE_BUCKETS];
    int size;                   /* Total number of key-value pairs stored */
} HashTable;

void hash_table_init(HashTable *ht);
bool hash_table_insert(HashTable *ht, int movie_id, int movie_index);
int hash_table_find(const HashTable *ht, int movie_id); /* Returns index or -1 if not found */
bool hash_table_remove(HashTable *ht, int movie_id);
void hash_table_clear(HashTable *ht);
void hash_table_build_from_list(HashTable *ht, const MovieList *list);

/* ========================================================================= */
/* 2. GRAPH (Adjacency List Representation)                                  */
/* Over the local set of loaded movies (up to MAX_MOVIES)                   */
/* Edges represent genre-overlap or recommendation relations                */
/* ========================================================================= */
typedef struct GraphEdgeNode {
    int dest;                   /* Destination vertex (index in MovieList) */
    int weight;                 /* Connection strength (e.g. number of shared genres) */
    struct GraphEdgeNode *next;
} GraphEdgeNode;

typedef struct {
    GraphEdgeNode *head;
} GraphAdjList;

typedef struct {
    int num_vertices;
    GraphAdjList adj[MAX_MOVIES];
} Graph;

void graph_init(Graph *g, int num_vertices);
void graph_add_edge(Graph *g, int src, int dest, int weight);
void graph_free(Graph *g);

/* Build edges based on shared genres between movies */
void graph_build_from_genres(Graph *g, const MovieList *list);

/* Build edges based on TMDB recommendation pairs */
void graph_add_recommendation_link(Graph *g, int src_index, int dest_index);

/* ========================================================================= */
/* 3. GRAPH TRAVERSAL: BFS & DFS                                            */
/* Time Complexity: O(V + E)                                                */
/* ========================================================================= */

/* BFS: Level-order breadth traversal to find closest related movies */
int graph_bfs(const Graph *g, int start_vertex, int *visited_order, int max_out);

/* DFS: Depth-first traversal exploring deep recommendation chains */
int graph_dfs(const Graph *g, int start_vertex, int *visited_order, int max_out);

/* Helper Queue for BFS */
typedef struct {
    int data[MAX_MOVIES];
    int front;
    int rear;
    int count;
} IntQueue;

void queue_init(IntQueue *q);
bool queue_is_empty(const IntQueue *q);
bool queue_is_full(const IntQueue *q);
bool queue_push(IntQueue *q, int value);
int queue_pop(IntQueue *q);

/* ========================================================================= */
/* 4. SORTING ALGORITHMS                                                     */
/* In-place sorting of MovieList items                                       */
/* ========================================================================= */

/* Insertion Sort: O(n^2) worst/avg, O(n) best. Sort by rating descending */
void sort_movies_by_rating_desc(MovieList *list);

/* Selection Sort: O(n^2). Sort by release date descending (newest first) */
void sort_movies_by_release_date_desc(MovieList *list);

/* Selection Sort: Sort by title alphabetically (A-Z) */
void sort_movies_by_title_asc(MovieList *list);

/* ========================================================================= */
/* 5. SEARCHING & FILTERING ALGORITHMS                                       */
/* ========================================================================= */

/* Linear search by case-insensitive title substring */
void filter_movies_by_title(const MovieList *src, MovieList *dest, const char *query);

/* Filter by genre ID */
void filter_movies_by_genre(const MovieList *src, MovieList *dest, int genre_id);

/* Filter by language code (ISO 639-1) */
void filter_movies_by_language(const MovieList *src, MovieList *dest, const char *lang);

/* Calculate number of shared genres between two movies */
int calculate_shared_genres(const Movie *a, const Movie *b);

#endif /* DS_H */
