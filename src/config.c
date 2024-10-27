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

gpointer *bow_setup_config(void) {
    gpointer config = g_new(struct bow_config, 1);

    // Only set at startup
    ((struct bow_config *)config)->buffer_size = 2048;
    ((struct bow_config *)config)->log_level = 1;

    // Only set at startup or at runtime
    ((struct bow_config *)config)->window_timeout = 600;

    // can be set at runtime
    ((struct bow_config *)config)->expression = NULL;
    ((struct bow_config *)config)->anchor = TOP_LEFT;
    ((struct bow_config *)config)->margin_left = 100;
    ((struct bow_config *)config)->margin_right = 0;
    ((struct bow_config *)config)->margin_top = 100;
    ((struct bow_config *)config)->margin_bottom = 0;

    bow_log_debug("config anchor = %d", ((struct bow_config *)config)->anchor);

    const char *log_level_env = getenv("BOW_LOG_LEVEL");
    if (log_level_env == NULL) {
        log_level_env = "INFO";
    }
    bow_log_level_set_from_string(log_level_env);

    const char *buffer_size_env = getenv("BOW_BUFFER_SIZE");
    if (buffer_size_env != NULL) {
        ((struct bow_config *)config)->buffer_size = atoi(buffer_size_env);
    }

    const char *window_timeout_env = getenv("BOW_WINDOW_TIMEOUT");
    if (window_timeout_env != NULL) {
        ((struct bow_config *)config)->window_timeout = atoi(window_timeout_env);
    }

    bow_log_info("Volume expression: %s", ((struct bow_config *)config)->expression);

    return config;
}
