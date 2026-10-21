# PRD — Product Requirements Document
**Project:** CineMatch — Movie Discovery & Recommendation CLI  
**Status:** LOCKED v1.0 | **Audience:** 3rd-semester C Data Structures mini project

## 1. Problem
A student wants to discover real movies by title, genre, language and rating without manually maintaining a large movie list. The project must demonstrate C-based Data Structures rather than outsourcing the logic to an AI service.

## 2. Product goal
Build a menu-driven C application that queries TMDB live and demonstrates search, filtering, sorting and graph-based related-movie recommendations.

## 3. Target user
A student or movie fan who wants to browse and filter movies from the TMDB catalogue.

## 4. V1 features — must have
- Fetch live movie results from TMDB.
- Search movies by title.
- Discover/filter by genre and original language.
- Display title, release year, rating, genres, overview (truncated if needed), and poster path/image URL text.
- Sort currently loaded results by rating.
- Use C structures and functions with readable comments.
- Show graceful errors for missing token, network failure, non-200 response, invalid/empty response.
- TMDB attribution visible in README and application help/about menu.

## 5. V1 features — should have if time permits
- Movie details by TMDB ID.
- Related movies using TMDB's recommendations endpoint.
- Build an adjacency-list graph from loaded movies using shared genres or TMDB similar/recommendation results; run BFS/DFS.
- Pagination / “load next page”.

## 6. Out of scope for v1
- Browser-based UI / website.
- Gemini API / generative AI.
- Login, favourites persisted to cloud, booking/ticket purchase.
- Scraping BookMyShow or other sites.
- Claiming complete coverage of all films ever released.
- Complex ML recommendation engine.

## 7. Success criteria
- Compiles using the documented command on the chosen environment.
- A real TMDB query returns and displays several movie records.
- Search and at least one genre/language filter work.
- Rating sort works.
- At least one DS feature is visibly demonstrable; target: hash table + graph/BFS if time allows.
- No secret/token committed to source control.
- Student can explain each implemented function in their own words.

## 8. Constraints and assumptions
- Requires internet access and a valid TMDB API credential.
- TMDB results are paginated; v1 displays only fetched page(s).
- C terminal UI is locked for speed and DS clarity. A graphical UI is a future extension.
