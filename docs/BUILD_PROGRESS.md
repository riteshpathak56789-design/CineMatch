# CineMatch — Build & Progress Log

## Milestone Status Overview

- [x] **M0: Environment & Toolchain Audit** (MSYS2 UCRT64 GCC 16.2.0, GNU Make 4.4.1, libcurl 8.23.0 verified)
- [x] **M1: Core Data Models, CLI Menu & Input Validation** (`src/movie.c`, `src/ui.c`, `src/main.c`)
- [x] **M2: TMDB HTTP Client** (libcurl, TLS handshake, error codes, URL encoding in `src/tmdb.c`)
- [x] **M3: JSON Parser** (jsmn token-boundary defensive parser in `src/json_parser.c`)
- [x] **M4: Movie Search by Title** (`tmdb_fetch_search` + local substring filter)
- [x] **M5: Genre & Language Filters** (`tmdb_fetch_discover` + local genre and ISO code filters)
- [x] **M6: Sorting Algorithms** (Insertion Sort for ratings, Selection Sort for dates and titles)
- [x] **M7: Recommendations & Movie Details Integration** (TMDB details and recommendations endpoints)
- [x] **M8: Graph Adjacency List & BFS/DFS Traversals** (`Graph`, `GraphEdgeNode`, `IntQueue`, `graph_bfs`, `graph_dfs`)
- [x] **M9: Hash Table** (Separate chaining hash table with 31 buckets for O(1) TMDB ID lookup)
- [x] **M10: Security, Memory Management & TMDB Attribution** (Attribution displayed, credentials never logged, no memory leaks)
- [x] **M11: Automated Test Suite & Smoke Testing** (90/90 tests passed across DS, JSON, and TMDB suites)
- [x] **M12: Viva Documentation & User Instruction Guide** (`docs/VIVA_NOTES.md` and `README.md`)

---

## Log Entries

### Entry 1: Environment Audit & Toolchain Setup (M0)
- **Toolchain**: Discovered MSYS2 installed at `C:\msys64`. Configured UCRT64 toolchain (GCC 16.2.0, GNU Make 4.4.1, libcurl 8.23.0 with OpenSSL 3.6.5).
- **Verified Linkage**: Successfully compiled and ran a test C11 binary linking `-lcurl`.
- **Project Structure**: Unpacked project specification documents (`PRD.md`, `TRD.md`, `DESIGN.md`, `SCHEMA.md`, `TRACKER.md`, `RULES.md`).

### Entry 2: Architecture & Implementation (M1 - M10)
- **Headers**: Created `include/movie.h`, `include/ds.h`, `include/tmdb.h`, `include/json_parser.h`, `include/ui.h`, and vendored `include/jsmn.h`.
- **Sources**: Implemented `src/movie.c`, `src/ds.c`, `src/tmdb.c`, `src/json_parser.c`, `src/ui.c`, and `src/main.c`.
- **Build System**: Created `Makefile`, `build.bat`, and `run.ps1` supporting clean, build, test, and run targets across CMD, PowerShell, and MSYS2.

### Entry 3: Automated Testing & Debugging (M11)
- Built automated test suites: `tests/test_ds.c` (49 assertions), `tests/test_json.c` (30 assertions), `tests/test_tmdb.c` (11 assertions), managed by `tests/test_runner.c`.
- Fixed JSON array token boundary skipping in `src/json_parser.c`.
- Ran full test suite: **90 / 90 tests passed (100% pass rate)**.
- Verified application executable `cinematch.exe` builds cleanly and executes non-interactively.

### Entry 4: Viva Documentation & Release Readiness (M12)
- Authored `docs/VIVA_NOTES.md` with data structure theory, complexities, function listings, and model viva answers.
- Updated `docs/TRACKER.md`.

---

## Next Step
- Prompt the user ONCE for the TMDB API Bearer Token to verify live authenticated queries (Section 5 of Master Build Prompt).
