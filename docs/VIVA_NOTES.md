# CineMatch — Comprehensive Viva & Technical Guide

This document is tailored for the 3rd-semester Data Structures (in C) viva examination. It breaks down the system architecture, theoretical foundations, time/space complexities, and every implemented function.

---

## 1. Project Overview & Architectural Flow

```
[TMDB REST API] 
       | (HTTPS GET via libcurl with TLS)
       v
[JSON Response Buffer (capped at 256KB)]
       | (Tokenized by jsmn parser)
       v
[Movie items[MAX_MOVIES] (Contiguous Memory Array)]
       |
       +---> [Hash Table (Separate Chaining)] -> O(1) ID Lookup
       +---> [Graph (Adjacency List)] ---------> Undirected Genre & Similarity Graph
       |                                          |-> BFS (Level-order recommendation)
       |                                          \-> DFS (Deep associative chain)
       +---> [Sorting Algorithms] -------------> Insertion Sort (Rating) & Selection Sort (Date)
       +---> [Linear Scan Filters] ------------> Substring, Genre ID, and Language Filters
       v
[Terminal UI (ANSI Color + Windows VT Console)]
```

---

## 2. Core Data Structures & Time Complexities

### A. Dynamic / Fixed Collection Array (`MovieList`)
- **Structure**: `Movie items[MAX_MOVIES]`, `int count`, `int page`, `int total_pages`.
- **Purpose**: Stores the current working set of movie records in contiguous memory.
- **Complexity**:
  - Access by index: **O(1)**
  - Cache Locality: **Optimal** (contiguous memory prevents CPU cache misses during sequential scans).

### B. Hash Table (`HashTable`)
- **Structure**: An array of `HashNode *buckets[31]`. Each node stores `(movie_id, movie_index, next)`.
- **Collision Resolution**: **Separate Chaining** (linked list at each bucket).
- **Hash Function**: Division method: `h(k) = k mod 31`. 31 is a prime number chosen to distribute TMDB integer IDs evenly across buckets.
- **Complexities**:
  - Insert: **Average O(1)**, **Worst O(N)** (if all keys collide).
  - Search (`hash_table_find`): **Average O(1)**, **Worst O(N)**.
  - Delete (`hash_table_remove`): **Average O(1)**, **Worst O(N)**.
  - Space: **O(M + N)** where M is bucket count (31) and N is items stored.

### C. Similarity Graph (`Graph`)
- **Structure**: **Adjacency List** using array `GraphAdjList adj[MAX_MOVIES]`. Each entry points to a singly linked list of `GraphEdgeNode` containing `dest`, `weight`, and `next`.
- **Semantics**:
  - Vertices: Movies loaded in memory (`0` to `count - 1`).
  - Edges: Undirected connection created when two movies share at least one genre.
  - Edge Weight: Number of shared genres (higher weight = stronger recommendation affinity).
- **Complexities**:
  - Space Complexity: **O(V + E)** (far more memory-efficient than an adjacency matrix of size V^2 for sparse graphs).
  - Edge Insertion: **O(deg(V))** to verify uniqueness and insert at head.

### D. Queue (`IntQueue`)
- **Structure**: Circular array `int data[MAX_MOVIES]`, `front`, `rear`, `count`.
- **Purpose**: First-In, First-Out (FIFO) ordering for Breadth-First Search.
- **Complexities**:
  - Enqueue (`queue_push`): **O(1)**
  - Dequeue (`queue_pop`): **O(1)**

---

## 3. Implemented Algorithms

### 1. Breadth-First Search (BFS)
- **Function**: `graph_bfs(g, start_vertex, visited_order, max_out)`
- **Mechanism**:
  1. Marks `start_vertex` as visited and enqueues it.
  2. While queue is not empty, dequeues vertex `u` and adds to output.
  3. Iterates through all neighbors of `u`. If an unvisited neighbor `v` is found, marks visited and enqueues it.
- **Time Complexity**: **O(V + E)**
- **Why BFS for Recommendations?** Level-order traversal guarantees discovering immediate 1st-degree neighbors (movies sharing genres directly with the seed) before exploring distant movies.

### 2. Depth-First Search (DFS)
- **Function**: `graph_dfs` / `dfs_recursive`
- **Mechanism**: Explores as far as possible along each branch before backtracking.
- **Time Complexity**: **O(V + E)**
- **Why DFS?** Discovers deep thematic and associational movie chains across genres.

### 3. Insertion Sort
- **Function**: `sort_movies_by_rating_desc(MovieList *list)`
- **Mechanism**: Incremental sort. Inserts each movie into its correct position within the already-sorted prefix.
- **Time Complexity**:
  - Best case (already sorted): **O(N)**
  - Average / Worst case: **O(N^2)**
- **Why Insertion Sort?** It is **stable** (preserves initial order of ties) and has minimal overhead for small collections (N <= 20).

### 4. Selection Sort
- **Function**: `sort_movies_by_release_date_desc` & `sort_movies_by_title_asc`
- **Mechanism**: Scans the remaining unsorted subarray to find the maximum/minimum element and swaps it with the current position.
- **Time Complexity**: **O(N^2)** comparisons, but exactly **O(N)** swaps.

---

## 4. Module-by-Module Function Directory

### `src/ds.c`
- `hash_func(key)`: Computes bucket index using modulo arithmetic.
- `hash_table_init(ht)`: Sets all bucket pointers to `NULL`, resets size to 0.
- `hash_table_insert(ht, movie_id, movie_index)`: Inserts new key-value pair or updates existing key.
- `hash_table_find(ht, movie_id)`: Traverses bucket list to return movie array index or -1.
- `hash_table_remove(ht, movie_id)`: Unlinks and frees node from collision chain.
- `hash_table_clear(ht)`: Traverses all buckets and frees all allocated `HashNode` memory.
- `hash_table_build_from_list(ht, list)`: Re-indexes entire movie list into hash table.
- `graph_init(g, num_vertices)`: Resets adjacency heads to `NULL`.
- `graph_add_edge(g, src, dest, weight)`: Inserts undirected edge with weight in both adjacency lists.
- `graph_free(g)`: Traverses and frees all edge nodes.
- `calculate_shared_genres(a, b)`: Compares genre IDs between two movies and returns overlap count.
- `graph_build_from_genres(g, list)`: O(N^2) pairwise comparison building the complete genre-overlap graph.
- `queue_init / push / pop / is_empty / is_full`: Safe bounded circular queue primitives.
- `graph_bfs(g, start, order, max)`: BFS level-order traversal.
- `graph_dfs(g, start, order, max)`: DFS depth-first recursive traversal.
- `sort_movies_by_rating_desc(list)`: In-place insertion sort by rating.
- `sort_movies_by_release_date_desc(list)`: In-place selection sort by date string.
- `sort_movies_by_title_asc(list)`: In-place selection sort alphabetically.
- `filter_movies_by_title(src, dest, query)`: Case-insensitive substring scan.
- `filter_movies_by_genre(src, dest, genre_id)`: Linear scan matching genre ID.
- `filter_movies_by_language(src, dest, lang)`: Linear scan matching language ISO code.

### `src/tmdb.c`
- `tmdb_global_init()`: Initializes libcurl networking engine.
- `tmdb_global_cleanup()`: Frees libcurl resources.
- `tmdb_has_token()`: Non-destructive check verifying `TMDB_BEARER_TOKEN` exists in environment.
- `tmdb_url_encode(input, out, out_size)`: Safely percent-encodes query strings using `curl_easy_escape`.
- `tmdb_http_get(url, require_auth, buf, max_size, http_code)`: Executes HTTPS request with SSL verification, timeout, bearer token header, and memory-safe write callback.
- `tmdb_fetch_popular / search / discover / details / recommendations`: High-level TMDB endpoint wrappers.

### `src/json_parser.c`
- `jsoneq(json, tok, s)`: Compares JSON string token against target string.
- `json_copy_string(json, tok, dest, size)`: Safely extracts string or maps `"null"` to `"N/A"`.
- `skip_token(tokens, index)`: O(1) character-boundary scanner that skips any token subtree (primitive, object, array).
- `parse_movie_object(json, tokens, obj_idx, m)`: Extracts ID, title, overview, rating, date, language, poster path, and genre arrays into `Movie`.
- `json_parse_movie_list(json, list)`: Parses paginated results array.
- `json_parse_single_movie(json, movie)`: Parses single movie details response.
- `json_parse_tmdb_error(json, msg, max)`: Extracts TMDB error message from error responses.

### `src/ui.c`
- `enable_windows_vt_mode()`: Enables Windows 10/11 ANSI VT console mode via Win32 API.
- `ui_clear_screen()`: Clears screen using ANSI escape sequences or fallback dashes.
- `ui_print_header / footer / attribution / menu / about`: Clean terminal layout and branding.
- `ui_display_movie_table(list)`: Renders tabular movie data with color-coded ratings.
- `ui_display_movie_card(m)`: Displays full formatted movie card with wrapped overview text.
- `ui_display_graph_structure(g, list)`: Visualizes adjacency list vertices, connections, and weights.
- `ui_display_traversal_results(algo, indices, count, list)`: Prints step-by-step traversal order.
- `ui_read_string / ui_read_int`: Robust input validation trimming newlines and rejecting invalid entries.

---

## 5. Typical Viva Questions & Model Answers

**Q1: Why did you choose separate chaining over open addressing for the hash table?**
*Answer*: Separate chaining handles collisions cleanly by maintaining linked lists at each bucket without suffering from primary or secondary clustering. It also allows simple deletions without requiring tombstone markers.

**Q2: What is the purpose of graph representation in CineMatch?**
*Answer*: The graph models semantic relationships between movies. Instead of treating movies as isolated rows in a database, each movie is a vertex, and edges represent shared genres. This allows us to use standard graph algorithms like BFS to compute natural recommendations.

**Q3: Why is Insertion Sort appropriate for sorting movie ratings here?**
*Answer*: Insertion sort is adaptive and stable. For small result sets (N <= 20) fetched per page, its overhead is lower than QuickSort or MergeSort, and it sorts an almost-sorted list in O(N) linear time.

**Q4: How do you prevent buffer overflow and memory leaks in C?**
*Answer*:
1. Fixed-size string buffers with `sizeof()` bounds in `snprintf` and `memcpy`.
2. Libcurl write callback tracks total byte count and terminates if the 256KB buffer limit is reached.
3. Every dynamic allocation (`malloc` for `HashNode` and `GraphEdgeNode`) has a matching cleanup function (`hash_table_clear`, `graph_free`) called whenever results are refreshed or when exiting.
