# Antigravity Start Prompt — Paste this first

You are helping implement CineMatch, a college Data Structures mini-project. Read `RULES.md` and all files in `docs/` before coding. Treat them as the locked project contract; if any requirement conflicts, `RULES.md` wins.

We need a C11 menu-driven terminal app that uses the official TMDB API via libcurl and parses JSON using a small C JSON tokenizer (jsmn is acceptable). No Python/JavaScript backend and no web UI in v1.

Work incrementally:
1. Inspect the environment first: OS, compiler, libcurl headers/library, and whether `TMDB_BEARER_TOKEN` exists. Do not print the token.
2. Report any missing dependency and the shortest installation path before writing much code.
3. Implement M1 and M2 only, compile, and test one TMDB endpoint.
4. Continue to M3 only after HTTP works. Then implement search/filter/sort.
5. Implement graph/BFS/DFS and hash table only after live API data displays.
6. Update `docs/TRACKER.md` honestly after each milestone.
7. Keep each function beginner-readable and explain how it works.
8. Never hardcode credentials. Never say tests passed unless actually run.
9. Preserve TMDB attribution.
10. Do not add features outside the locked scope.

Start by inspecting the environment and reporting a concise checklist. Do not generate a giant code dump before checking dependencies.
