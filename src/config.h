#ifndef CONFIG_H
#define CONFIG_H

#define RED "\033[0;31m"
#define BOLD_RED "\033[1;31m"
#define YELLOW "\033[0;33m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"
#define PURPLE "\033[0;35m"

#define FALLBACK_WINDOW_TIMEOUT 2500
#define FALLBACK_ANCHOR CENTER
#define FALLBACK_MARGIN_LEFT 100
#define FALLBACK_MARGIN_RIGHT 0
#define FALLBACK_MARGIN_TOP 100
#define FALLBACK_MARGIN_BOTTOM 0

#define MESSAGE_CURRENT_VERSION 0
#define PIPELAM_CURRENT_VERSION "v0.1.7"

#define FALLBACK_BOX_WIDTH 350
#define FALLBACK_BOX_HEIGHT 25
// Default colors for UI elements
#define FALLBACK_BORDER_COLOR "white"
#define FALLBACK_BACKGROUND_COLOR "black"
#define FALLBACK_FOREGROUND_COLOR "white"
#define FALLBACK_OVERFLOW_COLOR "red"

#define FALLBACK_BOX_COLOR "black"
#define FALLBACK_BOX_PADDING 0

// Default padding/margin settings for each component
#define FALLBACK_BORDER_PADDING 4
#define FALLBACK_BORDER_MARGIN 0
#define FALLBACK_BACKGROUND_PADDING 4
#define FALLBACK_FOREGROUND_PADDING 4
#define FALLBACK_FOREGROUND_OVERFLOW_PADDING 4

enum pipelam_window_anchor { BOTTOM_LEFT, BOTTOM_RIGHT, TOP_LEFT, TOP_RIGHT, CENTER };

enum pipelam_message_type { TEXT, IMAGE, WOB };

enum pipelam_runtime_behaviour { QUEUE, REPLACE, OVERLAY };

struct pipelam_config {
    // Startrup behaviour
    char *log_level;
    enum pipelam_runtime_behaviour runtime_behaviour;

    // Runtime behaviour
    // Message
    char *expression;
    enum pipelam_message_type type;
    int version;

    // Settings
    int window_timeout;
    enum pipelam_window_anchor anchor;

    int margin_left;
    int margin_right;
    int margin_top;
    int margin_bottom;

    // UI styling settings
    int box_width;
    int box_height;

    char *border_color;
    char *background_color;
    char *foreground_color;
    char *overflow_color;
    char *box_color;

    int box_padding;
    int border_padding;
    int border_margin;
    int background_padding;
    int foreground_padding;
    int foreground_overflow_padding;

    // Default values (set at startup from config/env)
    int default_window_timeout;
    enum pipelam_window_anchor default_anchor;

    int default_margin_left;
    int default_margin_right;
    int default_margin_top;
    int default_margin_bottom;

    int default_box_width;
    int default_box_height;

    char *default_border_color;
    char *default_background_color;
    char *default_foreground_color;
    char *default_overflow_color;
    char *default_box_color;

    int default_box_padding;
    int default_border_padding;
    int default_border_margin;
    int default_background_padding;
    int default_foreground_padding;
    int default_foreground_overflow_padding;
};

struct pipelam_config *pipelam_setup_config(const char *config_file_path);
void pipelam_destroy_config(struct pipelam_config *config);
void pipelam_reset_default_config(struct pipelam_config *config);
void pipelam_log_level_set_from_string(const char *log_level);

// taken from, altered slightly https://github.com/welljsjs/Config-Parser-C/blob/master/parser.h
#define CONFIG_ARG_MAX_BYTES 128

typedef struct pipelam_config_option pipelam_config_option;
typedef pipelam_config_option *pipelam_config_option_t;

struct pipelam_config_option {
    pipelam_config_option_t prev;
    char key[CONFIG_ARG_MAX_BYTES];
    char value[CONFIG_ARG_MAX_BYTES];
};

#endif // CONFIG_H
