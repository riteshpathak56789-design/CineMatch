#ifndef UI_H
#define UI_H

#include "movie.h"
#include "ds.h"
#include <stdbool.h>

/* ANSI Terminal Styling Codes */
#define ANSI_RESET   "\x1b[0m"
#define ANSI_BOLD    "\x1b[1m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_GRAY    "\x1b[90m"

/* Color enable/disable state */
void ui_set_color_enabled(bool enabled);
bool ui_is_color_enabled(void);

/* UI Screen and Banner components */
void ui_clear_screen(void);
void ui_print_header(const char *subtitle);
void ui_print_footer(void);
void ui_print_attribution(void);
void ui_print_main_menu(void);
void ui_print_about(void);

/* Result display */
void ui_display_movie_table(const MovieList *list);
void ui_display_movie_card(const Movie *m);
void ui_display_graph_structure(const Graph *g, const MovieList *list);
void ui_display_traversal_results(const char *algo_name, const int *indices, int count, const MovieList *list);

/* Safe input routines */
bool ui_read_string(const char *prompt, char *buffer, size_t buf_size);
bool ui_read_int(const char *prompt, int *value_out, int min_val, int max_val);
void ui_pause_for_user(void);

#endif /* UI_H */
