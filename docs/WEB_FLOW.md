# User Flow (V1)
**Note:** V1 is a terminal application, not a website. This is the locked interaction flow.

```text
START
  |
  v
Check TMDB_BEARER_TOKEN + initialize libcurl
  | failure -> show setup/help -> EXIT
  v
Main Menu
  1. Discover popular movies
  2. Search by title
  3. Filter by genre/language
  4. Sort last results by rating
  5. View movie details
  6. Recommend related movies
  7. About / TMDB attribution
  0. Exit
  |
  v
Call TMDB endpoint -> check HTTP status -> parse JSON
  | error -> friendly message -> Main Menu
  v
Store current page in Movie[] array
  |
  +--> Search/filter/sort local loaded results
  |
  +--> Select movie ID
          |
          +--> TMDB recommendations endpoint (if implemented)
          +--> create local graph among fetched results
          +--> BFS/DFS traversal (DS demonstration)
  |
  v
Display results -> return to Main Menu
```

## Input rules
- Trim newline from `fgets`.
- Reject empty queries and out-of-range menu choices.
- Never use unbounded `%s` for titles; titles contain spaces.
- Search results are limited to fetched pages. Offer page number only if pagination is implemented.
