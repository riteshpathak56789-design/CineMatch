#include "ds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ========================================================================= */
/* 1. HASH TABLE IMPLEMENTATION                                              */
/* ========================================================================= */

static int hash_func(int key) {
    /* Multiplicative or prime modulo hashing */
    int h = key % HASH_TABLE_BUCKETS;
    if (h < 0) h += HASH_TABLE_BUCKETS;
    return h;
}

void hash_table_init(HashTable *ht) {
    if (!ht) return;
    for (int i = 0; i < HASH_TABLE_BUCKETS; i++) {
        ht->buckets[i] = NULL;
    }
    ht->size = 0;
}

bool hash_table_insert(HashTable *ht, int movie_id, int movie_index) {
    if (!ht || movie_id <= 0) return false;

    int bucket = hash_func(movie_id);
    HashNode *curr = ht->buckets[bucket];

    /* Check if key already exists, update index */
    while (curr) {
        if (curr->movie_id == movie_id) {
            curr->movie_index = movie_index;
            return true;
        }
        curr = curr->next;
    }

    /* Allocate new node */
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    if (!new_node) return false;

    new_node->movie_id = movie_id;
    new_node->movie_index = movie_index;
    new_node->next = ht->buckets[bucket];
    ht->buckets[bucket] = new_node;
    ht->size++;
    return true;
}

int hash_table_find(const HashTable *ht, int movie_id) {
    if (!ht || movie_id <= 0) return -1;

    int bucket = hash_func(movie_id);
    HashNode *curr = ht->buckets[bucket];

    while (curr) {
        if (curr->movie_id == movie_id) {
            return curr->movie_index;
        }
        curr = curr->next;
    }
    return -1;
}

bool hash_table_remove(HashTable *ht, int movie_id) {
    if (!ht || movie_id <= 0) return false;

    int bucket = hash_func(movie_id);
    HashNode *curr = ht->buckets[bucket];
    HashNode *prev = NULL;

    while (curr) {
        if (curr->movie_id == movie_id) {
            if (prev) {
                prev->next = curr->next;
            } else {
                ht->buckets[bucket] = curr->next;
            }
            free(curr);
            ht->size--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

void hash_table_clear(HashTable *ht) {
    if (!ht) return;
    for (int i = 0; i < HASH_TABLE_BUCKETS; i++) {
        HashNode *curr = ht->buckets[i];
        while (curr) {
            HashNode *next = curr->next;
            free(curr);
            curr = next;
        }
        ht->buckets[i] = NULL;
    }
    ht->size = 0;
}

void hash_table_build_from_list(HashTable *ht, const MovieList *list) {
    if (!ht || !list) return;
    hash_table_clear(ht);
    for (int i = 0; i < list->count; i++) {
        hash_table_insert(ht, list->items[i].id, i);
    }
}

/* ========================================================================= */
/* 2. GRAPH & ADJACENCY LIST IMPLEMENTATION                                  */
/* ========================================================================= */

void graph_init(Graph *g, int num_vertices) {
    if (!g) return;
    if (num_vertices > MAX_MOVIES) {
        num_vertices = MAX_MOVIES;
    }
    g->num_vertices = num_vertices;
    for (int i = 0; i < MAX_MOVIES; i++) {
        g->adj[i].head = NULL;
    }
}

static bool edge_exists(const Graph *g, int src, int dest) {
    if (!g || src < 0 || src >= g->num_vertices) return false;
    GraphEdgeNode *curr = g->adj[src].head;
    while (curr) {
        if (curr->dest == dest) return true;
        curr = curr->next;
    }
    return false;
}

void graph_add_edge(Graph *g, int src, int dest, int weight) {
    if (!g || src < 0 || src >= g->num_vertices || dest < 0 || dest >= g->num_vertices || src == dest) {
        return;
    }

    /* Check if edge already exists */
    if (edge_exists(g, src, dest)) return;

    /* Add edge src -> dest */
    GraphEdgeNode *node1 = (GraphEdgeNode *)malloc(sizeof(GraphEdgeNode));
    if (!node1) return;
    node1->dest = dest;
    node1->weight = weight;
    node1->next = g->adj[src].head;
    g->adj[src].head = node1;

    /* Add undirected back-edge dest -> src */
    GraphEdgeNode *node2 = (GraphEdgeNode *)malloc(sizeof(GraphEdgeNode));
    if (!node2) return;
    node2->dest = src;
    node2->weight = weight;
    node2->next = g->adj[dest].head;
    g->adj[dest].head = node2;
}

void graph_free(Graph *g) {
    if (!g) return;
    for (int i = 0; i < MAX_MOVIES; i++) {
        GraphEdgeNode *curr = g->adj[i].head;
        while (curr) {
            GraphEdgeNode *next = curr->next;
            free(curr);
            curr = next;
        }
        g->adj[i].head = NULL;
    }
    g->num_vertices = 0;
}

int calculate_shared_genres(const Movie *a, const Movie *b) {
    if (!a || !b) return 0;
    int shared = 0;
    for (int i = 0; i < a->genre_count; i++) {
        for (int j = 0; j < b->genre_count; j++) {
            if (a->genre_ids[i] == b->genre_ids[j]) {
                shared++;
                break;
            }
        }
    }
    return shared;
}

void graph_build_from_genres(Graph *g, const MovieList *list) {
    if (!g || !list) return;
    graph_free(g);
    graph_init(g, list->count);

    for (int i = 0; i < list->count; i++) {
        for (int j = i + 1; j < list->count; j++) {
            int shared = calculate_shared_genres(&list->items[i], &list->items[j]);
            if (shared > 0) {
                graph_add_edge(g, i, j, shared);
            }
        }
    }
}

void graph_add_recommendation_link(Graph *g, int src_index, int dest_index) {
    /* Recommendation links are given high edge weight (e.g., 5) */
    graph_add_edge(g, src_index, dest_index, 5);
}

/* ========================================================================= */
/* 3. BFS & DFS TRAVERSALS                                                   */
/* ========================================================================= */

void queue_init(IntQueue *q) {
    if (!q) return;
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

bool queue_is_empty(const IntQueue *q) {
    return (q == NULL || q->count == 0);
}

bool queue_is_full(const IntQueue *q) {
    return (q != NULL && q->count >= MAX_MOVIES);
}

bool queue_push(IntQueue *q, int value) {
    if (!q || queue_is_full(q)) return false;
    q->rear = (q->rear + 1) % MAX_MOVIES;
    q->data[q->rear] = value;
    q->count++;
    return true;
}

int queue_pop(IntQueue *q) {
    if (!q || queue_is_empty(q)) return -1;
    int val = q->data[q->front];
    q->front = (q->front + 1) % MAX_MOVIES;
    q->count--;
    return val;
}

int graph_bfs(const Graph *g, int start_vertex, int *visited_order, int max_out) {
    if (!g || start_vertex < 0 || start_vertex >= g->num_vertices || !visited_order || max_out <= 0) {
        return 0;
    }

    bool visited[MAX_MOVIES] = {false};
    IntQueue q;
    queue_init(&q);

    visited[start_vertex] = true;
    queue_push(&q, start_vertex);

    int count = 0;
    while (!queue_is_empty(&q) && count < max_out) {
        int u = queue_pop(&q);
        visited_order[count++] = u;

        GraphEdgeNode *curr = g->adj[u].head;
        while (curr) {
            int v = curr->dest;
            if (v >= 0 && v < g->num_vertices && !visited[v]) {
                visited[v] = true;
                queue_push(&q, v);
            }
            curr = curr->next;
        }
    }
    return count;
}

static void dfs_recursive(const Graph *g, int u, bool *visited, int *visited_order, int *count, int max_out) {
    if (*count >= max_out) return;

    visited[u] = true;
    visited_order[(*count)++] = u;

    GraphEdgeNode *curr = g->adj[u].head;
    while (curr && *count < max_out) {
        int v = curr->dest;
        if (v >= 0 && v < g->num_vertices && !visited[v]) {
            dfs_recursive(g, v, visited, visited_order, count, max_out);
        }
        curr = curr->next;
    }
}

int graph_dfs(const Graph *g, int start_vertex, int *visited_order, int max_out) {
    if (!g || start_vertex < 0 || start_vertex >= g->num_vertices || !visited_order || max_out <= 0) {
        return 0;
    }

    bool visited[MAX_MOVIES] = {false};
    int count = 0;
    dfs_recursive(g, start_vertex, visited, visited_order, &count, max_out);
    return count;
}

/* ========================================================================= */
/* 4. SORTING ALGORITHMS                                                     */
/* ========================================================================= */

void sort_movies_by_rating_desc(MovieList *list) {
    if (!list || list->count <= 1) return;

    /* Insertion sort: stable and optimal for small N */
    for (int i = 1; i < list->count; i++) {
        Movie key = list->items[i];
        int j = i - 1;

        while (j >= 0 && list->items[j].rating < key.rating) {
            list->items[j + 1] = list->items[j];
            j--;
        }
        list->items[j + 1] = key;
    }
}

void sort_movies_by_release_date_desc(MovieList *list) {
    if (!list || list->count <= 1) return;

    /* Selection sort: find latest release date (YYYY-MM-DD string comparison) */
    for (int i = 0; i < list->count - 1; i++) {
        int max_idx = i;
        for (int j = i + 1; j < list->count; j++) {
            if (strcmp(list->items[j].release_date, list->items[max_idx].release_date) > 0) {
                max_idx = j;
            }
        }
        if (max_idx != i) {
            Movie temp = list->items[i];
            list->items[i] = list->items[max_idx];
            list->items[max_idx] = temp;
        }
    }
}

void sort_movies_by_title_asc(MovieList *list) {
    if (!list || list->count <= 1) return;

    for (int i = 0; i < list->count - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < list->count; j++) {
            if (strcasecmp(list->items[j].title, list->items[min_idx].title) < 0) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            Movie temp = list->items[i];
            list->items[i] = list->items[min_idx];
            list->items[min_idx] = temp;
        }
    }
}

/* ========================================================================= */
/* 5. SEARCHING & FILTERING                                                  */
/* ========================================================================= */

static bool string_contains_case_insensitive(const char *haystack, const char *needle) {
    if (!haystack || !needle) return false;
    if (needle[0] == '\0') return true;

    size_t h_len = strlen(haystack);
    size_t n_len = strlen(needle);
    if (n_len > h_len) return false;

    for (size_t i = 0; i <= h_len - n_len; i++) {
        size_t j = 0;
        while (j < n_len && tolower((unsigned char)haystack[i + j]) == tolower((unsigned char)needle[j])) {
            j++;
        }
        if (j == n_len) return true;
    }
    return false;
}

void filter_movies_by_title(const MovieList *src, MovieList *dest, const char *query) {
    if (!src || !dest) return;
    movie_list_init(dest);
    if (!query || query[0] == '\0') {
        *dest = *src;
        return;
    }

    for (int i = 0; i < src->count && dest->count < MAX_MOVIES; i++) {
        if (string_contains_case_insensitive(src->items[i].title, query)) {
            dest->items[dest->count++] = src->items[i];
        }
    }
}

void filter_movies_by_genre(const MovieList *src, MovieList *dest, int genre_id) {
    if (!src || !dest) return;
    movie_list_init(dest);

    for (int i = 0; i < src->count && dest->count < MAX_MOVIES; i++) {
        for (int g = 0; g < src->items[i].genre_count; g++) {
            if (src->items[i].genre_ids[g] == genre_id) {
                dest->items[dest->count++] = src->items[i];
                break;
            }
        }
    }
}

void filter_movies_by_language(const MovieList *src, MovieList *dest, const char *lang) {
    if (!src || !dest) return;
    movie_list_init(dest);
    if (!lang || lang[0] == '\0') {
        *dest = *src;
        return;
    }

    for (int i = 0; i < src->count && dest->count < MAX_MOVIES; i++) {
        if (strcasecmp(src->items[i].original_language, lang) == 0) {
            dest->items[dest->count++] = src->items[i];
        }
    }
}
