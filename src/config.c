#define _POSIX_C_SOURCE 200809L
#include <getopt.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "log.h"

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

void pipelam_help(void) {
    printf("Usage: pipelam [OPTIONS] <pipe_path>\n");
    printf("Options:\n");
    printf("  -l, --log-level=LEVEL        Set log level (DEBUG, INFO, WARNING, ERROR, PANIC)\n");
    printf("  -r, --runtime-behaviour=TYPE Set runtime behaviour (queue, replace, overlay)\n");
    printf("  -t, --window-timeout=MS      Set window timeout in milliseconds\n");
    printf("  -a, --anchor=POS             Set window anchor position (bottom-left, bottom-right, top-left, top-right, center)\n");
    printf("  -L, --margin-left=PIXELS     Set left margin in pixels\n");
    printf("  -R, --margin-right=PIXELS    Set right margin in pixels\n");
    printf("  -T, --margin-top=PIXELS      Set top margin in pixels\n");
    printf("  -B, --margin-bottom=PIXELS   Set bottom margin in pixels\n");
    printf("\n");
    printf("WOB bar options:\n");
    printf("  --wob-bar-height=PIXELS      Set WOB bar height in pixels\n");
    printf("  --wob-bar-width=PIXELS       Set WOB bar width in pixels\n");
    printf("  --wob-border-color=COLOR     Set WOB border color\n");
    printf("  --wob-background-color=COLOR Set WOB background color\n");
    printf("  --wob-foreground-color=COLOR Set WOB foreground color\n");
    printf("  --wob-overflow-color=COLOR   Set WOB overflow color\n");
    printf("  --wob-box-color=COLOR        Set WOB box color\n");
    printf("  --wob-box-padding=PIXELS     Set WOB box padding in pixels\n");
    printf("  --wob-border-padding=PIXELS  Set WOB border padding in pixels\n");
    printf("  --wob-border-margin=PIXELS   Set WOB border margin in pixels\n");
    printf("  --wob-background-padding=PIXELS Set WOB background padding in pixels\n");
    printf("  --wob-foreground-padding=PIXELS Set WOB foreground padding in pixels\n");
    printf("  --wob-foreground-overflow-padding=PIXELS Set WOB foreground overflow padding in pixels\n");
    printf("\n");
    printf("  -v, --version                Show version information\n");
    printf("  -h, --help                   Display this help message\n");
}

void pipelam_destroy_config(struct pipelam_config *config) { free(config); }

void pipelam_process_command_line_args(int argc, char *argv[], struct pipelam_config *config) {
    int opt;
    int option_index = 0;

    // Define the long options
    static struct option long_options[] = {{"log-level", required_argument, 0, 'l'},
                                           {"runtime-behaviour", required_argument, 0, 'r'},
                                           {"window-timeout", required_argument, 0, 't'},
                                           {"anchor", required_argument, 0, 'a'},
                                           {"margin-left", required_argument, 0, 'L'},
                                           {"margin-right", required_argument, 0, 'R'},
                                           {"margin-top", required_argument, 0, 'T'},
                                           {"margin-bottom", required_argument, 0, 'B'},
                                           {"wob-bar-height", required_argument, 0, 1001},
                                           {"wob-bar-width", required_argument, 0, 1002},
                                           {"wob-border-color", required_argument, 0, 1003},
                                           {"wob-background-color", required_argument, 0, 1004},
                                           {"wob-foreground-color", required_argument, 0, 1005},
                                           {"wob-overflow-color", required_argument, 0, 1006},
                                           {"wob-box-color", required_argument, 0, 1007},
                                           {"wob-box-padding", required_argument, 0, 1008},
                                           {"wob-border-padding", required_argument, 0, 1009},
                                           {"wob-border-margin", required_argument, 0, 1010},
                                           {"wob-background-padding", required_argument, 0, 1011},
                                           {"wob-foreground-padding", required_argument, 0, 1012},
                                           {"wob-foreground-overflow-padding", required_argument, 0, 1013},
                                           {"version", no_argument, 0, 'v'},
                                           {"help", no_argument, 0, 'h'},
                                           {0, 0, 0, 0}};

    // Reset getopt
    optind = 1;

    while ((opt = getopt_long(argc, argv, "l:r:t:a:L:R:T:B:vh", long_options, &option_index)) != -1) {
        switch (opt) {
        case 'l': // log-level
            config->log_level = strdup(optarg);
            break;

        case 'r': // runtime-behaviour
            if (strcmp(optarg, "queue") == 0) {
                config->runtime_behaviour = QUEUE;
            } else if (strcmp(optarg, "replace") == 0) {
                config->runtime_behaviour = REPLACE;
            } else if (strcmp(optarg, "overlay") == 0) {
                config->runtime_behaviour = OVERLAY;
            } else {
                pipelam_log_warning("Unknown runtime behaviour: %s", optarg);
            }
            break;

        case 't': // window-timeout
            config->window_timeout = atoi(optarg);
            config->default_window_timeout = config->window_timeout;
            break;

        case 'a': // anchor
            if (strcmp(optarg, "bottom-left") == 0) {
                config->anchor = BOTTOM_LEFT;
            } else if (strcmp(optarg, "bottom-right") == 0) {
                config->anchor = BOTTOM_RIGHT;
            } else if (strcmp(optarg, "top-left") == 0) {
                config->anchor = TOP_LEFT;
            } else if (strcmp(optarg, "top-right") == 0) {
                config->anchor = TOP_RIGHT;
            } else if (strcmp(optarg, "center") == 0) {
                config->anchor = CENTER;
            } else {
                pipelam_log_warning("Unknown anchor: %s", optarg);
            }
            config->default_anchor = config->anchor;
            break;

        case 'L': // margin-left
            config->margin_left = atoi(optarg);
            config->default_margin_left = config->margin_left;
            break;

        case 'R': // margin-right
            config->margin_right = atoi(optarg);
            config->default_margin_right = config->margin_right;
            break;

        case 'T': // margin-top
            config->margin_top = atoi(optarg);
            config->default_margin_top = config->margin_top;
            break;

        case 'B': // margin-bottom
            config->margin_bottom = atoi(optarg);
            config->default_margin_bottom = config->margin_bottom;
            break;

        case 1001: // wob-bar-height
            config->wob_bar_height = atoi(optarg);
            config->default_wob_bar_height = config->wob_bar_height;
            break;

        case 1002: // wob-bar-width
            config->wob_bar_width = atoi(optarg);
            config->default_wob_bar_width = config->wob_bar_width;
            break;

        case 1003: // wob-border-color
            config->wob_border_color = optarg;
            config->default_wob_border_color = optarg;
            break;

        case 1004: // wob-background-color
            config->wob_background_color = optarg;
            config->default_wob_background_color = optarg;
            break;

        case 1005: // wob-foreground-color
            config->wob_foreground_color = optarg;
            config->default_wob_foreground_color = optarg;
            break;

        case 1006: // wob-overflow-color
            config->wob_overflow_color = optarg;
            config->default_wob_overflow_color = optarg;
            break;

        case 1007: // wob-box-color
            config->wob_box_color = optarg;
            config->default_wob_box_color = optarg;
            break;

        case 1008: // wob-box-padding
            config->wob_box_padding = atoi(optarg);
            config->default_wob_box_padding = config->wob_box_padding;
            break;

        case 1009: // wob-border-padding
            config->wob_border_padding = atoi(optarg);
            config->default_wob_border_padding = config->wob_border_padding;
            break;

        case 1010: // wob-border-margin
            config->wob_border_margin = atoi(optarg);
            config->default_wob_border_margin = config->wob_border_margin;
            break;

        case 1011: // wob-background-padding
            config->wob_background_padding = atoi(optarg);
            config->default_wob_background_padding = config->wob_background_padding;
            break;

        case 1012: // wob-foreground-padding
            config->wob_foreground_padding = atoi(optarg);
            config->default_wob_foreground_padding = config->wob_foreground_padding;
            break;

        case 1013: // wob-foreground-overflow-padding
            config->wob_foreground_overflow_padding = atoi(optarg);
            config->default_wob_foreground_overflow_padding = config->wob_foreground_overflow_padding;
            break;

        case 'v': // version
            printf("Pipelam version %s\n", PIPELAM_CURRENT_VERSION);
            exit(EXIT_SUCCESS);

        case 'h': // help
            pipelam_help();
            exit(EXIT_SUCCESS);
            break;

        default:
            break;
        }
    }
}

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

void pipelam_override_from_environment(struct pipelam_config *config) {
    const char *log_level_env = getenv("PIPELAM_LOG_LEVEL");
    if (log_level_env != NULL) {
        config->log_level = (char *)log_level_env;
    }

    // anything other than info logs befor this point will only print log info as the level isn't set until this point
    // and the log level is set to info by default
    pipelam_log_level_set_from_string(config->log_level);

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

    while (1) {
        pipelam_config_option_t co = NULL;
        if ((co = calloc(1, sizeof(pipelam_config_option))) == NULL)
            continue;
        memset(co, 0, sizeof(pipelam_config_option));
        co->prev = last_co_addr;

        if (fscanf(fp, "%s = %s", &co->key[0], &co->value[0]) != 2) {
            if (feof(fp)) {
                break;
            }
            if (co->key[0] == '#') {
                while (fgetc(fp) != '\n') {
                    // Do nothing (to move the cursor to the end of the line).
                }
                free(co);
                continue;
            }
            // If we reach here, it means there was an error reading the file.
            pipelam_log_error("Unable to parse the config file %s", path);
            perror("fscanf()");
            free(co);
            continue;
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
    struct pipelam_config *config = g_new(struct pipelam_config, 1);

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

    // Initialize the actual WOB bar settings with defaults
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

    config->window_timeout = config->default_window_timeout;
    config->anchor = config->default_anchor;
    config->margin_left = config->default_margin_left;
    config->margin_right = config->default_margin_right;
    config->margin_top = config->default_margin_top;
    config->margin_bottom = config->default_margin_bottom;
    // WOB bar settings are set by the reset function call above

    config->expression = NULL;
    config->type = TEXT;
    config->version = MESSAGE_CURRENT_VERSION;

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
