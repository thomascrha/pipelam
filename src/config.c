#include "config.h"
#include <gtk/gtk.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

static void pipelam_log_level_set_from_string(const char *log_level) {
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
    // Reset all runtime configurable options to their custom default values before parsing new message
    config->window_timeout = config->default_window_timeout;
    config->anchor = config->default_anchor;
    config->margin_left = config->default_margin_left;
    config->margin_right = config->default_margin_right;
    config->margin_top = config->default_margin_top;
    config->margin_bottom = config->default_margin_bottom;
    config->expression = NULL;
    config->type = TEXT;
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
}

static pipelam_config_option_t pipelam_read_config_file(char *path) {
    FILE *fp;

    if ((fp = fopen(path, "r+")) == NULL) {
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
            perror("fscanf()");
            free(co);
            continue;
        }
        last_co_addr = co;
    }
    return last_co_addr;
}

static void pipelam_parse_config_file(char *path, struct pipelam_config *config) {
    pipelam_config_option_t co = pipelam_read_config_file(path);
    if (co == NULL) {
        perror("pipelam_read_config_file()");
        return;
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

        } else {
            pipelam_log_error("Unknown key: %s", co->key);
        }
        co = co->prev;
    }
}

static char *pipelam_get_config_file(const char *config_file_path) {
    // the precedence of the config file is as follows:
    // 1. Set by providing it explicitly to the function
    // 2. PIPELAM_CONFIG_FILE_PATH environment variable
    // 3. $HOME/.config/pipelam/config
    // 4. /etc/pipelam/config

    if (config_file_path != NULL) {
        return (char *)config_file_path;
    }

    const char *config_file_path_env = getenv("PIPELAM_CONFIG_FILE_PATH");
    if (config_file_path_env != NULL) {
        return (char *)config_file_path_env;
    }
    // ordered by priority
    char *paths[2] = {"$HOME/.config/pipelam/config", "/etc/pipelam/config"};
    for (int i = 0; i < 2; i++) {
        if (access(paths[i], F_OK) != -1) {
            return paths[i];
        }
    }

    return NULL;
}

struct pipelam_config *pipelam_setup_config(const char *config_file_path) {
    struct pipelam_config *config = g_new(struct pipelam_config, 1);

    // Only set at startup
    config->runtime_behaviour = FALLBACK_RUNTIME_BEHAVIOUR;
    config->log_level = FALLBACK_LOG_LEVEL;

    // Only set at startup or at runtime
    config->window_timeout = FALLBACK_WINDOW_TIMEOUT;

    // can be set at runtime
    config->expression = NULL;
    config->anchor = FALLBACK_ANCHOR;
    config->margin_left = FALLBACK_MARGIN_LEFT;
    config->margin_right = FALLBACK_MARGIN_RIGHT;
    config->margin_top = FALLBACK_MARGIN_TOP;
    config->margin_bottom = FALLBACK_MARGIN_BOTTOM;

    // Save these initial values as our default values
    config->default_window_timeout = FALLBACK_WINDOW_TIMEOUT;
    config->default_anchor = FALLBACK_ANCHOR;
    config->default_margin_left = FALLBACK_MARGIN_LEFT;
    config->default_margin_right = FALLBACK_MARGIN_RIGHT;
    config->default_margin_top = FALLBACK_MARGIN_TOP;
    config->default_margin_bottom = FALLBACK_MARGIN_BOTTOM;

    // order of precedence: config file, environment variables
    char *config_fp = pipelam_get_config_file(config_file_path);
    if (config_fp != NULL) {
        pipelam_parse_config_file(config_fp, config);
    } else {
        pipelam_log_warning("No config file found, using default values");
    }
    pipelam_override_from_environment(config);

    return config;
}
