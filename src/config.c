#include "config.h"
#include <gtk/gtk.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

static void bow_log_level_set_from_string(const char *log_level) {
    if (strcmp(log_level, "DEBUG") == 0) {
        bow_log_level_set(LOG_DEBUG);
    } else if (strcmp(log_level, "INFO") == 0) {
        bow_log_level_set(LOG_INFO);
    } else if (strcmp(log_level, "WARNING") == 0) {
        bow_log_level_set(LOG_WARNING);
    } else if (strcmp(log_level, "ERROR") == 0) {
        bow_log_level_set(LOG_ERROR);
    } else if (strcmp(log_level, "PANIC") == 0) {
        bow_log_level_set(LOG_PANIC);
    } else {
        bow_log_error("Unknown log level: %s", log_level);
    }
}

void bow_destroy_config(struct bow_config *config) { free(config); }

void bow_override_from_environment(struct bow_config *config) {
    bow_log_info("overriding config from environment");
    const char *log_level_env = getenv("BOW_LOG_LEVEL");
    if (log_level_env == NULL) {
        log_level_env = "INFO";
    }
    bow_log_info("log_level: %s", log_level_env);
    ((struct bow_config *)config)->log_level = (char *)log_level_env;
    bow_log_level_set_from_string(log_level_env);

    const char *buffer_size_env = getenv("BOW_BUFFER_SIZE");
    if (buffer_size_env != NULL) {
        bow_log_info("buffer_size: %d", ((struct bow_config *)config)->buffer_size);
        ((struct bow_config *)config)->buffer_size = atoi(buffer_size_env);
    }

    const char *window_timeout_env = getenv("BOW_WINDOW_TIMEOUT");
    if (window_timeout_env != NULL) {
        bow_log_info("log_level: %d", ((struct bow_config *)config)->log_level);
        bow_log_info("window_timeout: %d", ((struct bow_config *)config)->window_timeout);
        ((struct bow_config *)config)->window_timeout = atoi(window_timeout_env);
    }

    const char *anchor_env = getenv("BOW_ANCHOR");
    if (anchor_env != NULL) {
        bow_log_info("anchor: %d", ((struct bow_config *)config)->anchor);
        if (strcmp(anchor_env, "BOTTOM_LEFT") == 0) {
            ((struct bow_config *)config)->anchor = BOTTOM_LEFT;
        } else if (strcmp(anchor_env, "BOTTOM_RIGHT") == 0) {
            ((struct bow_config *)config)->anchor = BOTTOM_RIGHT;
        } else if (strcmp(anchor_env, "TOP_LEFT") == 0) {
            ((struct bow_config *)config)->anchor = TOP_LEFT;
        } else if (strcmp(anchor_env, "TOP_RIGHT") == 0) {
            ((struct bow_config *)config)->anchor = TOP_RIGHT;
        } else if (strcmp(anchor_env, "CENTER") == 0) {
            ((struct bow_config *)config)->anchor = CENTER;
        } else {
            bow_log_error("Unknown anchor: %s", anchor_env);
        }
    }

    const char *margin_left_env = getenv("BOW_MARGIN_LEFT");
    if (margin_left_env != NULL) {
        bow_log_info("margin_left: %d", ((struct bow_config *)config)->margin_left);
        ((struct bow_config *)config)->margin_left = atoi(margin_left_env);
    }

    const char *margin_right_env = getenv("BOW_MARGIN_RIGHT");
    if (margin_right_env != NULL) {
        bow_log_info("margin_right: %d", ((struct bow_config *)config)->margin_right);
        ((struct bow_config *)config)->margin_right = atoi(margin_right_env);
    }

    const char *margin_top_env = getenv("BOW_MARGIN_TOP");
    if (margin_top_env != NULL) {
        bow_log_info("margin_top: %d", ((struct bow_config *)config)->margin_top);
        ((struct bow_config *)config)->margin_top = atoi(margin_top_env);
    }

    const char *margin_bottom_env = getenv("BOW_MARGIN_BOTTOM");
    if (margin_bottom_env != NULL) {
        bow_log_info("margin_bottom: %d", ((struct bow_config *)config)->margin_bottom);
        ((struct bow_config *)config)->margin_bottom = atoi(margin_bottom_env);
    }
}

static bow_config_option_t bow_read_config_file(char *path) {
    FILE *fp;

    if ((fp = fopen(path, "r+")) == NULL) {
        perror("fopen()");
        return NULL;
    }

    bow_config_option_t last_co_addr = NULL;

    while (1) {
        bow_config_option_t co = NULL;
        if ((co = calloc(1, sizeof(bow_config_option))) == NULL)
            continue;
        memset(co, 0, sizeof(bow_config_option));
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

static void bow_parse_config_file(char *path, struct bow_config *config) {
    bow_config_option_t co = bow_read_config_file(path);
    if (co == NULL) {
        perror("bow_read_config_file()");
        return;
    }

    while (co != NULL) {
        if (strcmp(co->key, "buffer_size") == 0) {
            config->buffer_size = atoi(co->value);
        } else if (strcmp(co->key, "log_level") == 0) {
            config->log_level = co->value;
        } else if (strcmp(co->key, "window_timeout") == 0) {
            config->window_timeout = atoi(co->value);
        } else if (strcmp(co->key, "expression") == 0) {
            config->expression = co->value;
        } else if (strcmp(co->key, "anchor") == 0) {
            if (strcmp(co->value, "bottom-left") == 0) {
                config->anchor = BOTTOM_LEFT;
            } else if (strcmp(co->value, "bottom-right") == 0) {
                config->anchor = BOTTOM_RIGHT;
            } else if (strcmp(co->value, "top-left") == 0) {
                config->anchor = TOP_LEFT;
            } else if (strcmp(co->value, "top-right") == 0) {
                config->anchor = TOP_RIGHT;
            } else if (strcmp(co->value, "center") == 0) {
                config->anchor = CENTER;
            } else {
                bow_log_error("Unknown anchor: %s", co->value);
            }
        } else if (strcmp(co->key, "margin_left") == 0) {
            config->margin_left = atoi(co->value);
        } else if (strcmp(co->key, "margin_right") == 0) {
            config->margin_right = atoi(co->value);
        } else if (strcmp(co->key, "margin_top") == 0) {
            config->margin_top = atoi(co->value);
        } else if (strcmp(co->key, "margin_bottom") == 0) {
            config->margin_bottom = atoi(co->value);
        } else {
            bow_log_error("Unknown key: %s", co->key);
        }
        co = co->prev;
    }
}

static char *bow_get_config_file(void) {
    // the precedence of the config file is as follows:
    // 1. BOW_CONFIG_FILE_PATH environment variable
    // 2. $HOME/.config/bow/config
    // 3. /etc/bow/config
    const char *config_file_path_env = getenv("BOW_CONFIG_FILE_PATH");
    if (config_file_path_env != NULL) {
        return (char *)config_file_path_env;
    }
    // ordered by priority
    char *paths[2] = {"$HOME/.config/bow/config", "/etc/bow/config"};
    for (int i = 0; i < 2; i++) {
        if (access(paths[i], F_OK) != -1) {
            return paths[i];
        }
    }

    return NULL;
}

gpointer *bow_setup_config(void) {
    gpointer config = g_new(struct bow_config, 1);

    // Only set at startup
    ((struct bow_config *)config)->buffer_size = 2048;
    ((struct bow_config *)config)->log_level = "INFO";

    // Only set at startup or at runtime
    ((struct bow_config *)config)->window_timeout = 600;

    // can be set at runtime
    ((struct bow_config *)config)->expression = NULL;
    ((struct bow_config *)config)->anchor = TOP_LEFT;
    ((struct bow_config *)config)->margin_left = 100;
    ((struct bow_config *)config)->margin_right = 0;
    ((struct bow_config *)config)->margin_top = 100;
    ((struct bow_config *)config)->margin_bottom = 0;

    // order of precedence: config file, environment variables
    char *config_fp = bow_get_config_file();
    if (config_fp != NULL) {
        bow_log_info("found config file: %s", config_fp);
        bow_log_info("config->window_timeout: %d", ((struct bow_config *)config)->window_timeout);
        bow_parse_config_file(config_fp, config);
        bow_log_info("config->window_timeout: %d", ((struct bow_config *)config)->window_timeout);
    }
    bow_override_from_environment(config);

    return config;
}
