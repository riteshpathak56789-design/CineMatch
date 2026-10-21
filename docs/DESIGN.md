# UI / Interaction Design Spec
**Status:** LOCKED for V1. **Format:** terminal UI in C.

## Visual direction
Borrow the *information hierarchy* of modern movie-discovery products: clear heading, strong section labels, compact movie cards translated into terminal rows, and visible filters. Do not copy BookMyShow branding, logos, or exact proprietary screens.

### Palette (ANSI terminal where supported)
- Background: terminal default / dark terminal.
- Accent: red for headings and selected menu item.
- Gold/yellow: rating.
- Muted gray: metadata.
- Disable color automatically or allow `--no-color` if terminal doesn't support ANSI.

### Example layout
```text
========================================================
                    CINEMATCH
          Discover movies. Find your next watch.
========================================================
1. Popular    2. Search    3. Genre/Language    4. Sort
5. Details    6. Recommendations    7. About    0. Exit
--------------------------------------------------------
#  TITLE                         YEAR  LANG  RATING
1  Tumbbad                       2018  hi    8.2
2  Andhadhun                     2018  hi    8.2
3  Drishyam                      2015  hi    8.1
--------------------------------------------------------
Powered by TMDB | Not endorsed or certified by TMDB
```

## Result-row design
- Show a maximum of 10–20 results per screen.
- Title left-aligned and truncated safely.
- Rating displayed to one decimal place.
- Language as ISO code (e.g. hi, en, ta, te) or mapped to readable label.
- Posters are not rendered in a normal terminal; display poster URL/path as an optional detail field. Poster-rich graphical UI is explicitly future scope.

## Why not web UI in v1?
A C-only web app would require a web server, routing, frontend assets, and deployment choices. That is a different scope and risks the 60–90 minute goal. The design goal is presentable terminal output plus clear DS demonstrations.

## Visual references
- TMDB API docs (fields and image paths): https://developer.themoviedb.org/reference/intro/getting-started
- TMDB image URL basics: https://developer.themoviedb.org/docs/image-basics
- Discover/search endpoint patterns: https://developer.themoviedb.org/reference/discover-movie
- The user-provided reference video was not visually legible in the sampled frames available during planning; no specific screen details are claimed from it.
