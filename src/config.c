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
    } else {
        pipelam_log_error("Unknown log level: %s", log_level);
    }
}

void pipelam_destroy_config(struct pipelam_config *config) { free(config); }

void pipelam_override_from_environment(struct pipelam_config *config) {
    pipelam_log_debug("overriding config from environment");
    const char *log_level_env = getenv("PIPELAM_LOG_LEVEL");
    if (log_level_env == NULL) {
        log_level_env = "INFO";
    }
    pipelam_log_debug("log_level: %s", log_level_env);
    ((struct pipelam_config *)config)->log_level = (char *)log_level_env;
    pipelam_log_level_set_from_string(log_level_env);

    const char *max_message_size_env = getenv("PIPELAME_MAX_MESSAGE_SIZE");
    if (max_message_size_env != NULL) {
        pipelam_log_debug("max_message_size: %d", ((struct pipelam_config *)config)->max_message_size);
        int max_message_size = atoi(max_message_size_env);
        ((struct pipelam_config *)config)->max_message_size = max_message_size;
    }

    const char *runtime_behaviour_env = getenv("PIPELAM_RUNTIME_BEHAVIOUR");
    if (runtime_behaviour_env != NULL) {
        pipelam_log_debug("runtime_behaviour: %d", ((struct pipelam_config *)config)->runtime_behaviour);
        enum pipelam_runtime_behaviour runtime_behaviour_val = ((struct pipelam_config *)config)->runtime_behaviour;
        if (strcmp(runtime_behaviour_env, "queue") == 0) {
            runtime_behaviour_val = QUEUE;
        } else if (strcmp(runtime_behaviour_env, "replace") == 0) {
            runtime_behaviour_val = REPLACE;
        } else {
            pipelam_log_error("Unknown runtime behaviour: %s", runtime_behaviour_env);
        }
        ((struct pipelam_config *)config)->runtime_behaviour = runtime_behaviour_val;
    }

    const char *window_timeout_env = getenv("PIPELAM_WINDOW_TIMEOUT");
    if (window_timeout_env != NULL) {
        pipelam_log_debug("log_level: %d", ((struct pipelam_config *)config)->log_level);
        pipelam_log_debug("window_timeout: %d", ((struct pipelam_config *)config)->window_timeout);
        int timeout = atoi(window_timeout_env);
        ((struct pipelam_config *)config)->window_timeout = timeout;
        ((struct pipelam_config *)config)->default_window_timeout = timeout;
    }

    const char *anchor_env = getenv("PIPELAM_ANCHOR");
    if (anchor_env != NULL) {
        pipelam_log_debug("anchor: %d", ((struct pipelam_config *)config)->anchor);
        enum pipelam_window_anchor anchor_val = ((struct pipelam_config *)config)->anchor;
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
        ((struct pipelam_config *)config)->anchor = anchor_val;
        ((struct pipelam_config *)config)->default_anchor = anchor_val;
    }

    const char *margin_left_env = getenv("PIPELAM_MARGIN_LEFT");
    if (margin_left_env != NULL) {
        pipelam_log_debug("margin_left: %d", ((struct pipelam_config *)config)->margin_left);
        int margin = atoi(margin_left_env);
        ((struct pipelam_config *)config)->margin_left = margin;
        ((struct pipelam_config *)config)->default_margin_left = margin;
    }

    const char *margin_right_env = getenv("PIPELAM_MARGIN_RIGHT");
    if (margin_right_env != NULL) {
        pipelam_log_debug("margin_right: %d", ((struct pipelam_config *)config)->margin_right);
        int margin = atoi(margin_right_env);
        ((struct pipelam_config *)config)->margin_right = margin;
        ((struct pipelam_config *)config)->default_margin_right = margin;
    }

    const char *margin_top_env = getenv("PIPELAM_MARGIN_TOP");
    if (margin_top_env != NULL) {
        pipelam_log_debug("margin_top: %d", ((struct pipelam_config *)config)->margin_top);
        int margin = atoi(margin_top_env);
        ((struct pipelam_config *)config)->margin_top = margin;
        ((struct pipelam_config *)config)->default_margin_top = margin;
    }

    const char *margin_bottom_env = getenv("PIPELAM_MARGIN_BOTTOM");
    if (margin_bottom_env != NULL) {
        pipelam_log_debug("margin_bottom: %d", ((struct pipelam_config *)config)->margin_bottom);
        int margin = atoi(margin_bottom_env);
        ((struct pipelam_config *)config)->margin_bottom = margin;
        ((struct pipelam_config *)config)->default_margin_bottom = margin;
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
        if (strcmp(co->key, "max_message_size") == 0) {
            config->max_message_size = atoi(co->value);
        } else if (strcmp(co->key, "log_level") == 0) {
            config->log_level = co->value;
        } else if (strcmp(co->key, "runtime_behaviour") == 0) {
            if (strcmp(co->value, "queue") == 0) {
                config->runtime_behaviour = QUEUE;
            } else if (strcmp(co->value, "replace") == 0) {
                config->runtime_behaviour = REPLACE;
            } else {
                pipelam_log_error("Unknown runtime behaviour: %s", co->value);
            }
        } else if (strcmp(co->key, "max_message_size") == 0) {
            config->max_message_size = atoi(co->value);
        } else if (strcmp(co->key, "window_timeout") == 0) {
            int timeout = atoi(co->value);
            config->window_timeout = timeout;
            config->default_window_timeout = timeout;
        } else if (strcmp(co->key, "expression") == 0) {
            config->expression = co->value;
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

static char *pipelam_get_config_file(void) {
    // the precedence of the config file is as follows:
    // 1. pipelam_CONFIG_FILE_PATH environment variable
    // 2. $HOME/.config/bow/config
    // 3. /etc/bow/config
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

gpointer *pipelam_setup_config(void) {
    gpointer config = g_new(struct pipelam_config, 1);

    // Only set at startup
    ((struct pipelam_config *)config)->runtime_behaviour = FALLBACK_RUNTIME_BEHAVIOUR;
    ((struct pipelam_config *)config)->log_level = FALLBACK_LOG_LEVEL;
    ((struct pipelam_config *)config)->max_message_size = FALLBACK_MAX_MESSAGE_SIZE;

    // Only set at startup or at runtime
    ((struct pipelam_config *)config)->window_timeout = FALLBACK_WINDOW_TIMEOUT;

    // can be set at runtime
    ((struct pipelam_config *)config)->expression = NULL;
    ((struct pipelam_config *)config)->anchor = FALLBACK_ANCHOR;
    ((struct pipelam_config *)config)->margin_left = FALLBACK_MARGIN_LEFT;
    ((struct pipelam_config *)config)->margin_right = FALLBACK_MARGIN_RIGHT;
    ((struct pipelam_config *)config)->margin_top = FALLBACK_MARGIN_TOP;
    ((struct pipelam_config *)config)->margin_bottom = FALLBACK_MARGIN_BOTTOM;

    // Save these initial values as our default values
    ((struct pipelam_config *)config)->default_window_timeout = FALLBACK_WINDOW_TIMEOUT;
    ((struct pipelam_config *)config)->default_anchor = FALLBACK_ANCHOR;
    ((struct pipelam_config *)config)->default_margin_left = FALLBACK_MARGIN_LEFT;
    ((struct pipelam_config *)config)->default_margin_right = FALLBACK_MARGIN_RIGHT;
    ((struct pipelam_config *)config)->default_margin_top = FALLBACK_MARGIN_TOP;
    ((struct pipelam_config *)config)->default_margin_bottom = FALLBACK_MARGIN_BOTTOM;

    // order of precedence: config file, environment variables
    char *config_fp = pipelam_get_config_file();
    if (config_fp != NULL) {
        pipelam_log_debug("found config file: %s", config_fp);
        pipelam_parse_config_file(config_fp, config);
    }
    pipelam_override_from_environment(config);

    return config;
}
