# CineMatch — C Data Structures Mini Project

**Locked direction:** C language core + TMDB API + terminal UI for the first submission-ready version.  
**Timebox:** 60–90 minutes for the first working slice; integration/debugging may extend this.  
**Do not silently switch the core implementation to Python, Java, or JavaScript.**

## Why terminal UI?
The subject is Data Structures in C and the time limit is tight. A polished terminal menu is reliable, demonstrable, and lets the viva focus stay on the data structures. A graphical website would add a separate frontend/backend and is explicitly out of scope for v1.

## Build plan
1. Install/check `gcc`, `libcurl`, and a small JSON parser such as `jsmn`.
2. Set `TMDB_BEARER_TOKEN` as an environment variable.
3. Build and run the C CLI.
4. Search TMDB for real movies; parse and display results.
5. Filter by genre/language and sort by rating.
6. Build a graph over the movies currently loaded in the result set; use TMDB similar-movies endpoint if time permits, otherwise genre-overlap edges.
7. Test failure cases and prepare a short viva explanation.

## Important scope truth
TMDB is a remote catalogue, not a guarantee of literally every movie ever made. Results are paginated and limited by the API response/page settings. The application must say “results from TMDB,” not “every existing movie.”

## TMDB attribution
Display in README and app output:
“This product uses the TMDB API but is not endorsed or certified by TMDB.”

## Official references
- TMDB API intro: https://developer.themoviedb.org/reference/intro/getting-started
- TMDB application authentication: https://developer.themoviedb.org/docs/authentication-application
- TMDB discover movies: https://developer.themoviedb.org/reference/discover-movie
- TMDB search movies: https://developer.themoviedb.org/reference/search-movie
- TMDB genres: https://developer.themoviedb.org/reference/genre-movie-list
- TMDB recommendations: https://developer.themoviedb.org/reference/movie-recommendations
- libcurl tutorial (C): https://curl.se/libcurl/c/libcurl-tutorial.html
- jsmn JSON parser: https://github.com/zserge/jsmn
