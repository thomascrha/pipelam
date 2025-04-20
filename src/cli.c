#define _POSIX_C_SOURCE 200809L
#include "cli.h"
#include "config.h"
#include "log.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
