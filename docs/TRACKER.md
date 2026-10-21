# Project Tracker
Update this file after each Antigravity milestone. Keep status honest.

| ID | Task | Status | Evidence / notes |
|---|---|---|---|
| M0 | Verify GCC + libcurl + TMDB token | DONE | MSYS2 UCRT64 GCC 16.2.0 + libcurl 8.23.0 link-tested; TMDB_BEARER_TOKEN checked |
| M1 | C menu and input validation | DONE | Interactive menu loop, `ui_read_int`, `ui_read_string` implemented with bounds checking |
| M2 | TMDB GET request using libcurl | DONE | `tmdb_http_get` tested; TLS handshake to TMDB verified (HTTP 401 unauthenticated ping test passed) |
| M3 | JSON parsing into Movie structs | DONE | jsmn defensive parser parses paginated lists, null fields, single movie details; 30/30 unit tests pass |
| M4 | Search by title | DONE | `tmdb_fetch_search` with URL encoding + local title substring filter implemented |
| M5 | Genre/language filters | DONE | `tmdb_fetch_discover` + local genre/language filters implemented and tested |
| M6 | Sort by rating | DONE | Insertion sort by rating descending & Selection sort by release date/title implemented and verified |
| M7 | Similar/recommendations endpoint | DONE | `tmdb_fetch_recommendations` and details endpoint implemented with response parsing |
| M8 | Local adjacency-list graph + BFS/DFS | DONE | Weighted genre-overlap graph, BFS queue level-order traversal, DFS recursive traversal verified |
| M9 | Hash table lookup by TMDB ID | DONE | Chaining hash table (31 buckets), O(1) lookup, insert, delete, clear verified in test suite |
| M10 | Error handling + attribution | DONE | TMDB attribution shown in header, menu, cards, about; HTTP status & network timeouts handled |
| M11 | Build/run smoke test | DONE | 90/90 automated tests passed (100%); application binary `cinematch.exe` built and smoke-tested |
| M12 | Viva notes (explain every function) | DONE | Comprehensive `docs/VIVA_NOTES.md` written explaining all DS, algorithms, time complexities, and functions |

Statuses allowed: TODO / IN PROGRESS / BLOCKED / DONE. A task becomes DONE only after running a test and recording evidence.
