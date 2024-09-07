#include "config.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

int bow_log_level_set_from_string(const char *log_level) {
    bow_log_info("bow_log_level_set_from_string() called with log_level: %s", log_level);
    if (strcmp(log_level, "DEBUG") == 0) {
        bow_log_level_set(LOG_DEBUG);
        return LOG_DEBUG;
    } else if (strcmp(log_level, "INFO") == 0) {
        bow_log_level_set(LOG_INFO);
        return LOG_INFO;
    } else if (strcmp(log_level, "WARNING") == 0) {
        bow_log_level_set(LOG_WARNING);
        return LOG_WARNING;
    } else if (strcmp(log_level, "ERROR") == 0) {
        bow_log_level_set(LOG_ERROR);
        return LOG_ERROR;
    } else if (strcmp(log_level, "PANIC") == 0) {
        bow_log_level_set(LOG_PANIC);
        return LOG_PANIC;
    }

    bow_log_warning("Unknown log level: %s defaulting to INFO", log_level);
    return LOG_INFO;
}

void bow_destroy_config(struct bow_config *config) {
    bow_log_debug("bow_destroy_config() called");
    free(config);
}

struct bow_config *bow_setup_config(void) {
    bow_log_debug("bow_setup_config() called");
    struct bow_config *config = malloc(sizeof(struct bow_config));
    if (config == NULL) {
        bow_log_error("Failed to allocate memory for bow_config");
        return NULL;
    }

    config->buffer_size = BOW_BUFFER_SIZE;
    config->log_level = BOW_LOG_LEVEL;
    config->window_timeout = BOW_WINDOW_TIMEOUT;

    const char *log_level_env = getenv("BOW_LOG_LEVEL");
    if (log_level_env == NULL) {
        printf("log_level_env is NULL\n");
        log_level_env = "INFO";
    }

    int log_level_no = bow_log_level_set_from_string(log_level_env);
    bow_log_level_set(log_level_no);

    const char *buffer_size_env = getenv("BOW_BUFFER_SIZE");
    if (buffer_size_env != NULL) {
        config->buffer_size = atoi(buffer_size_env);
    }

    const char *window_timeout_env = getenv("BOW_WINDOW_TIMEOUT");
    if (window_timeout_env != NULL) {
        config->window_timeout = atoi(window_timeout_env);
    }

    return config;
}
