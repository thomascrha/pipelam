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

#define FALLBACK_WOB_BAR_WIDTH 350
#define FALLBACK_WOB_BAR_HEIGHT 25
// Default colors for WOB bar
#define FALLBACK_WOB_BORDER_COLOR "white"
#define FALLBACK_WOB_BACKGROUND_COLOR "black"
#define FALLBACK_WOB_FOREGROUND_COLOR "white"
#define FALLBACK_WOB_OVERFLOW_COLOR "red"

#define FALLBACK_WOB_BOX_COLOR "black"
#define FALLBACK_WOB_BOX_PADDING 0

// Default padding/margin settings for each component
#define FALLBACK_WOB_BORDER_PADDING 4
#define FALLBACK_WOB_BORDER_MARGIN 4
#define FALLBACK_WOB_BACKGROUND_PADDING 4
#define FALLBACK_WOB_FOREGROUND_PADDING 4
#define FALLBACK_WOB_FOREGROUND_OVERFLOW_PADDING 4

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

    // wob specific settings
    int wob_bar_width;
    int wob_bar_height;

    char *wob_border_color;
    char *wob_background_color;
    char *wob_foreground_color;
    char *wob_overflow_color;
    char *wob_box_color;

    int wob_box_padding;
    int wob_border_padding;
    int wob_border_margin;
    int wob_background_padding;
    int wob_foreground_padding;
    int wob_foreground_overflow_padding;

    // Default values (set at startup from config/env)
    int default_window_timeout;
    enum pipelam_window_anchor default_anchor;

    int default_margin_left;
    int default_margin_right;
    int default_margin_top;
    int default_margin_bottom;

    int default_wob_bar_width;
    int default_wob_bar_height;

    char *default_wob_border_color;
    char *default_wob_background_color;
    char *default_wob_foreground_color;
    char *default_wob_overflow_color;
    char *default_wob_box_color;

    int default_wob_box_padding;
    int default_wob_border_padding;
    int default_wob_border_margin;
    int default_wob_background_padding;
    int default_wob_foreground_padding;
    int default_wob_foreground_overflow_padding;
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
