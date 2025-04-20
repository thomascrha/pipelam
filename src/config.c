#define _POSIX_C_SOURCE 200809L
#include "config.h"
#include "cli.h"
#include "log.h"
#include <ctype.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void pipelam_log_level_set_from_string(const char *log_level) {
    if (strcmp(log_level, "DEBUG") == 0) {
        pipelam_log_level_set(LOG_DEBUG);
    } else if (strcmp(log_level, "INFO") == 0) {
        pipelam_log_level_set(LOG_INFO);
    } else if (strcmp(log_level, "WARNING") == 0) {
        pipelam_log_level_set(LOG_WARNING);
    } else if (strcmp(log_level, "ERROR") == 0) {
        pipelam_log_level_set(LOG_ERROR);
    } else if (strcmp(log_level, "PANIC") == 0) {
        pipelam_log_level_set(LOG_PANIC);
    } else if (strcmp(log_level, "TEST") == 0) {
        pipelam_log_level_set(LOG_TEST);
    } else {
        pipelam_log_error("Unknown log level: %s", log_level);
    }
}

void pipelam_destroy_config(struct pipelam_config *config) { free(config); }

void pipelam_reset_default_config(struct pipelam_config *config) {
    config->window_timeout = config->default_window_timeout;
    config->anchor = config->default_anchor;
    config->margin_left = config->default_margin_left;
    config->margin_right = config->default_margin_right;
    config->margin_top = config->default_margin_top;
    config->margin_bottom = config->default_margin_bottom;

    config->wob_bar_height = config->default_wob_bar_height;
    config->wob_bar_width = config->default_wob_bar_width;
    config->wob_border_color = config->default_wob_border_color;
    config->wob_background_color = config->default_wob_background_color;
    config->wob_foreground_color = config->default_wob_foreground_color;
    config->wob_overflow_color = config->default_wob_overflow_color;
    config->wob_box_color = config->default_wob_box_color;
    config->wob_box_padding = config->default_wob_box_padding;
    config->wob_border_padding = config->default_wob_border_padding;
    config->wob_border_margin = config->default_wob_border_margin;
    config->wob_background_padding = config->default_wob_background_padding;
    config->wob_foreground_padding = config->default_wob_foreground_padding;
    config->wob_foreground_overflow_padding = config->default_wob_foreground_overflow_padding;

    config->expression = NULL;
    config->type = TEXT;
    config->version = MESSAGE_CURRENT_VERSION;
}

static void pipelam_set_log_level_from_env(struct pipelam_config *config) {
    pipelam_log_debug("Overridng Log Level from Environment");
    const char *log_level_env = getenv("PIPELAM_LOG_LEVEL");
    if (log_level_env != NULL) {
        config->log_level = (char *)log_level_env;
    }

    // anything other than info logs befor this point will only print log info as the level isn't set until this point
    // and the log level is set to info by default
    pipelam_log_level_set_from_string(config->log_level);
    pipelam_log_debug("log_level: %s", config->log_level);
    pipelam_log_debug("backgorund_color: %s", config->wob_background_color);
    pipelam_log_debug("forground_color: %s", config->wob_foreground_color);
}

void pipelam_override_from_environment(struct pipelam_config *config) {


    const char *runtime_behaviour_env = getenv("PIPELAM_RUNTIME_BEHAVIOUR");
    if (runtime_behaviour_env != NULL) {
        pipelam_log_debug("runtime_behaviour: %d", config->runtime_behaviour);
        enum pipelam_runtime_behaviour runtime_behaviour_val = config->runtime_behaviour;
        if (strcmp(runtime_behaviour_env, "queue") == 0) {
            runtime_behaviour_val = QUEUE;
        } else if (strcmp(runtime_behaviour_env, "replace") == 0) {
            runtime_behaviour_val = REPLACE;
        } else if (strcmp(runtime_behaviour_env, "overlay") == 0) {
            runtime_behaviour_val = OVERLAY;
        } else {
            pipelam_log_error("Unknown runtime behaviour: %s", runtime_behaviour_env);
        }
        config->runtime_behaviour = runtime_behaviour_val;
    }

    const char *window_timeout_env = getenv("PIPELAM_WINDOW_TIMEOUT");
    if (window_timeout_env != NULL) {
        pipelam_log_debug("log_level: %s", config->log_level);
        pipelam_log_debug("window_timeout: %d", config->window_timeout);
        int timeout = atoi(window_timeout_env);
        config->window_timeout = timeout;
        config->default_window_timeout = timeout;
    }

    const char *anchor_env = getenv("PIPELAM_ANCHOR");
    if (anchor_env != NULL) {
        pipelam_log_debug("anchor: %d", config->anchor);
        enum pipelam_window_anchor anchor_val = config->anchor;
        if (strcmp(anchor_env, "BOTTOM_LEFT") == 0) {
            anchor_val = BOTTOM_LEFT;
        } else if (strcmp(anchor_env, "BOTTOM_RIGHT") == 0) {
            anchor_val = BOTTOM_RIGHT;
        } else if (strcmp(anchor_env, "TOP_LEFT") == 0) {
            anchor_val = TOP_LEFT;
        } else if (strcmp(anchor_env, "TOP_RIGHT") == 0) {
            anchor_val = TOP_RIGHT;
        } else if (strcmp(anchor_env, "CENTER") == 0) {
            anchor_val = CENTER;
        } else {
            pipelam_log_error("Unknown anchor: %s", anchor_env);
        }
        config->anchor = anchor_val;
        config->default_anchor = anchor_val;
    }

    const char *margin_left_env = getenv("PIPELAM_MARGIN_LEFT");
    if (margin_left_env != NULL) {
        pipelam_log_debug("margin_left: %d", config->margin_left);
        int margin = atoi(margin_left_env);
        config->margin_left = margin;
        config->default_margin_left = margin;
    }

    const char *margin_right_env = getenv("PIPELAM_MARGIN_RIGHT");
    if (margin_right_env != NULL) {
        pipelam_log_debug("margin_right: %d", config->margin_right);
        int margin = atoi(margin_right_env);
        config->margin_right = margin;
        config->default_margin_right = margin;
    }

    const char *margin_top_env = getenv("PIPELAM_MARGIN_TOP");
    if (margin_top_env != NULL) {
        pipelam_log_debug("margin_top: %d", config->margin_top);
        int margin = atoi(margin_top_env);
        config->margin_top = margin;
        config->default_margin_top = margin;
    }

    const char *margin_bottom_env = getenv("PIPELAM_MARGIN_BOTTOM");
    if (margin_bottom_env != NULL) {
        pipelam_log_debug("margin_bottom: %d", config->margin_bottom);
        int margin = atoi(margin_bottom_env);
        config->margin_bottom = margin;
        config->default_margin_bottom = margin;
    }

    const char *wob_bar_width_env = getenv("PIPELAM_WOB_BAR_WIDTH");
    if (wob_bar_width_env != NULL) {
        pipelam_log_debug("wob_bar_width: %d", config->wob_bar_width);
        int width = atoi(wob_bar_width_env);
        config->wob_bar_width = width;
        config->default_wob_bar_width = width;
    }

    const char *wob_bar_height_env = getenv("PIPELAM_WOB_BAR_HEIGHT");
    if (wob_bar_height_env != NULL) {
        pipelam_log_debug("wob_bar_height: %d", config->wob_bar_height);
        int height = atoi(wob_bar_height_env);
        config->wob_bar_height = height;
        config->default_wob_bar_height = height;
    }

    const char *wob_border_color_env = getenv("PIPELAM_WOB_BORDER_COLOR");
    if (wob_border_color_env != NULL) {
        pipelam_log_debug("wob_border_color: %s", wob_border_color_env);
        config->wob_border_color = (char *)wob_border_color_env;
        config->default_wob_border_color = (char *)wob_border_color_env;
    }

    const char *wob_background_color_env = getenv("PIPELAM_WOB_BACKGROUND_COLOR");
    if (wob_background_color_env != NULL) {
        pipelam_log_debug("wob_background_color: %s", wob_background_color_env);
        config->wob_background_color = (char *)wob_background_color_env;
        config->default_wob_background_color = (char *)wob_background_color_env;
    }

    const char *wob_foreground_color_env = getenv("PIPELAM_WOB_FOREGROUND_COLOR");
    if (wob_foreground_color_env != NULL) {
        pipelam_log_debug("wob_foreground_color: %s", wob_foreground_color_env);
        config->wob_foreground_color = (char *)wob_foreground_color_env;
        config->default_wob_foreground_color = (char *)wob_foreground_color_env;
    }

    const char *wob_overflow_color_env = getenv("PIPELAM_WOB_OVERFLOW_COLOR");
    if (wob_overflow_color_env != NULL) {
        pipelam_log_debug("wob_overflow_color: %s", wob_overflow_color_env);
        config->wob_overflow_color = (char *)wob_overflow_color_env;
        config->default_wob_overflow_color = (char *)wob_overflow_color_env;
    }

    const char *wob_box_color_env = getenv("PIPELAM_WOB_BOX_COLOR");
    if (wob_box_color_env != NULL) {
        pipelam_log_debug("wob_box_color: %s", wob_box_color_env);
        config->wob_box_color = (char *)wob_box_color_env;
        config->default_wob_box_color = (char *)wob_box_color_env;
    }

    const char *wob_box_padding_env = getenv("PIPELAM_WOB_BOX_PADDING");
    if (wob_box_padding_env != NULL) {
        pipelam_log_debug("wob_box_padding: %s", wob_box_padding_env);
        int wob_box_padding = atoi(wob_box_padding_env);
        config->wob_box_padding = wob_box_padding;
        config->default_wob_box_padding = wob_box_padding;
    }

    const char *wob_border_padding_env = getenv("PIPELAM_WOB_BORDER_PADDING");
    if (wob_border_padding_env != NULL) {
        pipelam_log_debug("wob_border_padding: %s", wob_border_padding_env);
        int border_padding = atoi(wob_border_padding_env);
        config->wob_border_padding = border_padding;
        config->default_wob_border_padding = border_padding;
    }

    const char *wob_border_margin_env = getenv("PIPELAM_WOB_BORDER_MARGIN");
    if (wob_border_margin_env != NULL) {
        pipelam_log_debug("wob_border_margin: %s", wob_border_margin_env);
        int border_margin = atoi(wob_border_margin_env);
        config->wob_border_margin = border_margin;
        config->default_wob_border_margin = border_margin;
    }

    const char *wob_background_padding_env = getenv("PIPELAM_WOB_BACKGROUND_PADDING");
    if (wob_background_padding_env != NULL) {
        pipelam_log_debug("wob_background_padding: %s", wob_background_padding_env);
        int background_padding = atoi(wob_background_padding_env);
        config->wob_background_padding = background_padding;
        config->default_wob_background_padding = background_padding;
    }

    const char *wob_foreground_padding_env = getenv("PIPELAM_WOB_FOREGROUND_PADDING");
    if (wob_foreground_padding_env != NULL) {
        pipelam_log_debug("wob_foreground_padding: %s", wob_foreground_padding_env);
        int foreground_padding = atoi(wob_foreground_padding_env);
        config->wob_foreground_padding = foreground_padding;
        config->default_wob_foreground_padding = foreground_padding;
    }

    const char *wob_foreground_overflow_padding_env = getenv("PIPELAM_WOB_FOREGROUND_OVERFLOW_PADDING");
    if (wob_foreground_overflow_padding_env != NULL) {
        pipelam_log_debug("wob_foreground_overflow_padding: %s", wob_foreground_overflow_padding_env);
        int foreground_overflow_padding = atoi(wob_foreground_overflow_padding_env);
        config->wob_foreground_overflow_padding = foreground_overflow_padding;
        config->default_wob_foreground_overflow_padding = foreground_overflow_padding;
    }
}

static pipelam_config_option_t pipelam_read_config_file(char *path) {
    // taken from, altered slightly https://github.com/welljsjs/Config-Parser-C/blob/master/parser.h
    FILE *fp;

    if ((fp = fopen(path, "r")) == NULL) {
        perror("fopen()");
        return NULL;
    }

    pipelam_config_option_t last_co_addr = NULL;
    char line[1024]; // Buffer for reading each line

    while (fgets(line, sizeof(line), fp) != NULL) {
        pipelam_config_option_t co = NULL;
        if ((co = calloc(1, sizeof(pipelam_config_option))) == NULL)
            continue;
        memset(co, 0, sizeof(pipelam_config_option));
        co->prev = last_co_addr;

        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || strspn(line, " \t\r\n") == strlen(line)) {
            free(co);
            continue;
        }

        // Also skip lines that are indented and start with a #, which are likely comment continuations
        if (isspace(line[0]) && strchr(line, '#') != NULL) {
            free(co);
            continue;
        }

        // Parse key
        char *key_start = line;
        char *key_end = strchr(line, '=');
        if (key_end == NULL) {
            // Check if this might be a continuation of a comment
            if (strstr(line, "#") != NULL) {
                free(co);
                continue;
            }
            pipelam_log_error("Malformed line in config file (missing '='): %s", line);
            free(co);
            continue;
        }

        // Check if there's a comment on this line
        char *comment = strchr(line, '#');
        if (comment != NULL && comment < key_end) {
            // This is a comment line that happens to have an equals sign in it
            free(co);
            continue;
        }

        // Trim trailing whitespace from key
        char *key_trim = key_end - 1;
        while (key_trim >= key_start && isspace(*key_trim)) {
            key_trim--;
        }

        // Copy key
        int key_len = key_trim - key_start + 1;
        if (key_len > CONFIG_ARG_MAX_BYTES - 1) {
            key_len = CONFIG_ARG_MAX_BYTES - 1;
        }
        strncpy(co->key, key_start, key_len);
        co->key[key_len] = '\0';

        // Parse value
        char *value_start = key_end + 1;
        // Skip leading whitespace
        while (*value_start && isspace(*value_start)) {
            value_start++;
        }

        // Check for quoted value
        if (*value_start == '"') {
            value_start++; // Skip opening quote
            char *value_end = strchr(value_start, '"');
            if (value_end == NULL) {
                pipelam_log_error("Malformed line in config file (unclosed quote): %s", line);
                free(co);
                continue;
            }

            int value_len = value_end - value_start;
            if (value_len > CONFIG_ARG_MAX_BYTES - 1) {
                value_len = CONFIG_ARG_MAX_BYTES - 1;
            }
            strncpy(co->value, value_start, value_len);
            co->value[value_len] = '\0';
        } else {
            // Unquoted value - trim trailing whitespace and remove comments
            char *comment = strchr(value_start, '#');
            char *value_end;

            if (comment != NULL) {
                // There's a comment in the value part - trim before it
                value_end = comment - 1;
            } else {
                value_end = value_start + strlen(value_start) - 1;
            }

            // Trim trailing whitespace
            while (value_end >= value_start && (isspace(*value_end) || *value_end == '\n')) {
                value_end--;
            }

            int value_len = value_end - value_start + 1;
            if (value_len > CONFIG_ARG_MAX_BYTES - 1) {
                value_len = CONFIG_ARG_MAX_BYTES - 1;
            }
            strncpy(co->value, value_start, value_len);
            co->value[value_len] = '\0';
        }

        last_co_addr = co;
    }
    return last_co_addr;
}

static bool pipelam_parse_config_file(char *path, struct pipelam_config *config) {
    pipelam_config_option_t co = pipelam_read_config_file(path);
    if (co == NULL) {
        perror("pipelam_read_config_file()");
        return FALSE;
    }

    while (co != NULL) {
        if (strcmp(co->key, "log_level") == 0) {
            config->log_level = co->value;
            pipelam_log_level_set_from_string(config->log_level);
        } else if (strcmp(co->key, "runtime_behaviour") == 0) {
            if (strcmp(co->value, "queue") == 0) {
                config->runtime_behaviour = QUEUE;
            } else if (strcmp(co->value, "replace") == 0) {
                config->runtime_behaviour = REPLACE;
            } else if (strcmp(co->value, "overlay") == 0) {
                config->runtime_behaviour = OVERLAY;
            } else {
                pipelam_log_error("Unknown runtime behaviour: %s", co->value);
            }
        } else if (strcmp(co->key, "window_timeout") == 0) {
            int timeout = atoi(co->value);
            config->window_timeout = timeout;
            config->default_window_timeout = timeout;
        } else if (strcmp(co->key, "anchor") == 0) {
            enum pipelam_window_anchor anchor_val = config->anchor;
            if (strcmp(co->value, "bottom-left") == 0) {
                anchor_val = BOTTOM_LEFT;
            } else if (strcmp(co->value, "bottom-right") == 0) {
                anchor_val = BOTTOM_RIGHT;
            } else if (strcmp(co->value, "top-left") == 0) {
                anchor_val = TOP_LEFT;
            } else if (strcmp(co->value, "top-right") == 0) {
                anchor_val = TOP_RIGHT;
            } else if (strcmp(co->value, "center") == 0) {
                anchor_val = CENTER;
            } else {
                pipelam_log_error("Unknown anchor: %s", co->value);
            }
            config->anchor = anchor_val;
            config->default_anchor = anchor_val;
        } else if (strcmp(co->key, "margin_left") == 0) {
            int margin = atoi(co->value);
            config->margin_left = margin;
            config->default_margin_left = margin;
        } else if (strcmp(co->key, "margin_right") == 0) {
            int margin = atoi(co->value);
            config->margin_right = margin;
            config->default_margin_right = margin;
        } else if (strcmp(co->key, "margin_top") == 0) {
            int margin = atoi(co->value);
            config->margin_top = margin;
            config->default_margin_top = margin;
        } else if (strcmp(co->key, "margin_bottom") == 0) {
            int margin = atoi(co->value);
            config->margin_bottom = margin;
            config->default_margin_bottom = margin;
        } else if (strcmp(co->key, "wob_bar_width") == 0) {
            int width = atoi(co->value);
            config->wob_bar_width = width;
            config->default_wob_bar_width = width;
        } else if (strcmp(co->key, "wob_bar_height") == 0) {
            int height = atoi(co->value);
            config->wob_bar_height = height;
            config->default_wob_bar_height = height;

        } else if (strcmp(co->key, "wob_border_color") == 0) {
            config->wob_border_color = co->value;
            config->default_wob_border_color = co->value;
        } else if (strcmp(co->key, "wob_background_color") == 0) {
            config->wob_background_color = co->value;
            config->default_wob_background_color = co->value;
        } else if (strcmp(co->key, "wob_foreground_color") == 0) {
            config->wob_foreground_color = co->value;
            config->default_wob_foreground_color = co->value;
        } else if (strcmp(co->key, "wob_overflow_color") == 0) {
            config->wob_overflow_color = co->value;
            config->default_wob_overflow_color = co->value;
        } else if (strcmp(co->key, "wob_box_color") == 0) {
            config->wob_box_color = co->value;
            config->default_wob_box_color = co->value;
        } else if (strcmp(co->key, "wob_box_padding") == 0) {
            int box_padding = atoi(co->value);
            config->wob_box_padding = box_padding;
            config->default_wob_box_padding = box_padding;
        } else if (strcmp(co->key, "wob_border_padding") == 0) {
            int border_padding = atoi(co->value);
            config->wob_border_padding = border_padding;
            config->default_wob_border_padding = border_padding;
        } else if (strcmp(co->key, "wob_border_margin") == 0) {
            int border_margin = atoi(co->value);
            config->wob_border_margin = border_margin;
            config->default_wob_border_margin = border_margin;
        } else if (strcmp(co->key, "wob_background_padding") == 0) {
            int background_padding = atoi(co->value);
            config->wob_background_padding = background_padding;
            config->default_wob_background_padding = background_padding;
        } else if (strcmp(co->key, "wob_foreground_padding") == 0) {
            int foreground_padding = atoi(co->value);
            config->wob_foreground_padding = foreground_padding;
            config->default_wob_foreground_padding = foreground_padding;
        } else if (strcmp(co->key, "wob_foreground_overflow_padding") == 0) {
            int foreground_overflow_padding = atoi(co->value);
            config->wob_foreground_overflow_padding = foreground_overflow_padding;
            config->default_wob_foreground_overflow_padding = foreground_overflow_padding;
        } else {
            pipelam_log_error("Unknown key: %s", co->key);
        }
        co = co->prev;
    }

    return TRUE;
}

static char *pipelam_get_config_file(const char *config_file_path) {
    // the precedence of the config file is as follows:
    // 1. Set by providing it explicitly to the function
    // 2. PIPELAM_CONFIG_FILE_PATH environment variable
    // 3. Check PIPELAM_SKIP_DEFAULT_CONFIG - if set, skip default locations - used for testing
    // 4. $HOME/.config/pipelam/pipelam.ini
    // 5. /etc/pipelam/pipelam.toml

    if (config_file_path != NULL) {
        return (char *)config_file_path;
    }

    const char *config_file_path_env = getenv("PIPELAM_CONFIG_FILE_PATH");
    if (config_file_path_env != NULL) {
        return (char *)config_file_path_env;
    }

    // Check if we should skip default config files (used for testing)
    const char *skip_default_config = getenv("PIPELAM_SKIP_DEFAULT_CONFIG");
    if (skip_default_config != NULL) {
        pipelam_log_debug("Skipping default config files due to PIPELAM_SKIP_DEFAULT_CONFIG");
        return NULL;
    }

    // ordered by priority
    char *paths[2] = {"$HOME/.config/pipelam/pipelam.toml", "/etc/pipelam/pipelam.toml"};
    for (int i = 0; i < 2; i++) {
        if (access(paths[i], F_OK) != -1) {
            return paths[i];
        }
    }

    return NULL;
}

struct pipelam_config *pipelam_setup_config(const char *config_file_path) {
    struct pipelam_config *config = g_new0(struct pipelam_config, 1);  // Use g_new0 to zero-initialize

    // Initialise everything from code then override with config file then environment
    config->runtime_behaviour = REPLACE;
    config->log_level = "INFO";

    config->default_window_timeout = FALLBACK_WINDOW_TIMEOUT;
    config->default_anchor = FALLBACK_ANCHOR;
    config->default_margin_left = FALLBACK_MARGIN_LEFT;
    config->default_margin_right = FALLBACK_MARGIN_RIGHT;
    config->default_margin_top = FALLBACK_MARGIN_TOP;
    config->default_margin_bottom = FALLBACK_MARGIN_BOTTOM;

    config->default_wob_bar_height = FALLBACK_WOB_BAR_HEIGHT;
    config->default_wob_bar_width = FALLBACK_WOB_BAR_WIDTH;
    config->default_wob_border_color = FALLBACK_WOB_BORDER_COLOR;
    config->default_wob_background_color = FALLBACK_WOB_BACKGROUND_COLOR;
    config->default_wob_foreground_color = FALLBACK_WOB_FOREGROUND_COLOR;
    config->default_wob_overflow_color = FALLBACK_WOB_OVERFLOW_COLOR;
    config->default_wob_box_color = FALLBACK_WOB_BOX_COLOR;
    config->default_wob_box_padding = FALLBACK_WOB_BOX_PADDING;
    config->default_wob_border_padding = FALLBACK_WOB_BORDER_PADDING;
    config->default_wob_border_margin = FALLBACK_WOB_BORDER_MARGIN;
    config->default_wob_background_padding = FALLBACK_WOB_BACKGROUND_PADDING;
    config->default_wob_foreground_padding = FALLBACK_WOB_FOREGROUND_PADDING;
    config->default_wob_foreground_overflow_padding = FALLBACK_WOB_FOREGROUND_OVERFLOW_PADDING;

    config->wob_bar_height = FALLBACK_WOB_BAR_HEIGHT;
    config->wob_bar_width = FALLBACK_WOB_BAR_WIDTH;
    config->wob_border_color = FALLBACK_WOB_BORDER_COLOR;
    config->wob_background_color = FALLBACK_WOB_BACKGROUND_COLOR;
    config->wob_foreground_color = FALLBACK_WOB_FOREGROUND_COLOR;
    config->wob_overflow_color = FALLBACK_WOB_OVERFLOW_COLOR;
    config->wob_box_color = FALLBACK_WOB_BOX_COLOR;
    config->wob_box_padding = FALLBACK_WOB_BOX_PADDING;
    config->wob_border_padding = FALLBACK_WOB_BORDER_PADDING;
    config->wob_border_margin = FALLBACK_WOB_BORDER_MARGIN;
    config->wob_background_padding = FALLBACK_WOB_BACKGROUND_PADDING;
    config->wob_foreground_padding = FALLBACK_WOB_FOREGROUND_PADDING;
    config->wob_foreground_overflow_padding = FALLBACK_WOB_FOREGROUND_OVERFLOW_PADDING;

    config->window_timeout = FALLBACK_WINDOW_TIMEOUT;
    config->anchor = FALLBACK_ANCHOR;
    config->margin_left = FALLBACK_MARGIN_LEFT;
    config->margin_right = FALLBACK_MARGIN_RIGHT;
    config->margin_top = FALLBACK_MARGIN_TOP;
    config->margin_bottom = FALLBACK_MARGIN_BOTTOM;
    // WOB bar settings are set by the reset function call above

    config->expression = NULL;
    config->type = TEXT;
    config->version = MESSAGE_CURRENT_VERSION;


    pipelam_set_log_level_from_env(config);

     // order of precedence: config file, environment variables
    char *config_fp = pipelam_get_config_file(config_file_path);
    if (config_fp != NULL) {
        bool config_parsed = pipelam_parse_config_file(config_fp, config);
        if (config_parsed == FALSE) {
            pipelam_log_warning("Unable to parse config file %s, using default values", config_fp);
        }
    } else {
        pipelam_log_warning("No config file found, using default values");
    }

    pipelam_override_from_environment(config);

    return config;
}
