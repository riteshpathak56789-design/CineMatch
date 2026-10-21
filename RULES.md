# RULES.md — Non-Negotiable Project Rules
**CineMatch v1.0 — LOCKED**

1. **Language lock:** Core program and Data Structures must be C (C11). Do not silently introduce Python, Node.js, Java, or a framework backend.
2. **API lock:** Use official TMDB API endpoints; do not scrape BookMyShow or other sites.
3. **Credential safety:** Read `TMDB_BEARER_TOKEN` from environment. Never hardcode or print it. Never commit secrets.
4. **Scope lock:** V1 is a menu-driven terminal app. No web frontend, GUI, database, user login, Gemini, or booking system until core acceptance checks pass.
5. **Honesty lock:** Do not claim every movie ever made is available. Say “TMDB results” and note pagination.
6. **DS integrity:** Implement the data structures in C yourself. Explain what is actually implemented; no fake claims.
7. **Readable code:** Small functions, meaningful names, comments for non-obvious logic, bounds checks, no unsafe unbounded input.
8. **Compile quality:** Use `-std=c11 -Wall -Wextra -pedantic` where supported. Fix warnings that indicate real bugs.
9. **Memory safety:** Check allocation results, free allocated memory, cap JSON and result buffers, validate indices, avoid buffer overflows.
10. **HTTP safety:** URL-encode user input; set a timeout; check libcurl errors and HTTP status; handle null fields and empty results.
11. **No fake success:** If API parsing, graph traversal, or hash lookup is not tested, mark it incomplete in TRACKER.md.
12. **Incremental work:** Build and run after each milestone. Do not generate the entire app in one giant untested pass.
13. **Attribution:** Include “This product uses the TMDB API but is not endorsed or certified by TMDB.”
14. **No scope creep:** Only add optional features after M0–M5 are passing.
15. **Student understanding:** Keep code simple enough to explain in a viva; write short notes for every function.
