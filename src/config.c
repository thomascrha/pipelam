#include "config.h"
#include "log.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int bow_config_buffer_size = BUFFER_SIZE;
int bow_config_log_level = LOG_INFO;
int bow_config_window_timeout = WINDOW_TIMEOUT;

static void bow_log_level_set(int log_level) {
	bow_config_log_level = log_level;
}

static void bow_buffer_size_set(int buffer_size) {
	bow_config_buffer_size = buffer_size;
}

static void bow_window_timeout_set(int window_timeout) {
	bow_config_window_timeout = window_timeout;
}

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

void bow_setup_config(void) {
	const char *log_level_env = getenv("BOW_LOG_LEVEL");
	if (log_level_env == NULL) {
		log_level_env = "INFO";
	}
	bow_log_level_set_from_string(log_level_env);

	const char *buffer_size_env = getenv("BOW_BUFFER_SIZE");
	if (buffer_size_env != NULL) {
		bow_buffer_size_set(atoi(buffer_size_env));
	}

	const char *window_timeout_env = getenv("BOW_WINDOW_TIMEOUT");
	if (window_timeout_env != NULL) {
		bow_window_timeout_set(atoi(window_timeout_env));
	}

}
