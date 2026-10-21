# Implementation Milestones & Timebox
**Target:** first working slice in 60–90 minutes; no guarantee if machine dependencies/API credentials are not ready.

| ID | Milestone | Timebox | Acceptance check | Priority |
|---|---|---:|---|---|
| M0 | Verify GCC, libcurl, token | 5–10 min | compiler/dependency check passes; token env var exists | P0 |
| M1 | Minimal C CLI + menu | 5–10 min | app launches, menu repeats, exit works | P0 |
| M2 | HTTP request via libcurl | 10–15 min | one endpoint returns HTTP 200 | P0 |
| M3 | Parse JSON into Movie[] | 15–20 min | at least 5 real results displayed | P0 |
| M4 | Search + genre/language filters | 10–15 min | query and one filter work | P0 |
| M5 | Rating sort | 5–10 min | result order changes correctly | P1 |
| M6 | Related movies + local graph BFS/DFS | 15–25 min | graph traversal displays connected IDs/titles | P1 |
| M7 | Hash table + errors + README/viva | 15–25 min | ID lookup works; failures handled | P1 |
| M8 | Polish and test | 10–20 min | compile warnings/errors reviewed; demo flow passes | P0 |
| M9 | Optional GUI | NOT IN V1 | only after core is complete | P3 |

**Critical path:** M0 -> M1 -> M2 -> M3.  
**Cut line at 90 min:** Must have M1–M3 working; prioritize search/filter and sorting next. Do not pretend graph/hash table is done if omitted.  
**Big risk:** dependency setup on Windows. If libcurl is missing, spend no more than 10 minutes on setup before choosing a preinstalled environment (e.g. WSL/MSYS2) or documenting the blocker.
