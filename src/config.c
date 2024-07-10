#include "config.h"
#include "log.h"

int buffer_size = BUFFER_SIZE;
int log_level = LOG_INFO;
int window_timeout = WINDOW_TIMEOUT;

void bow_log_level_set(int importance) {
	log_level = importance;
}

void bow_buffer_size_set(int size) {
	buffer_size = size;
}

void bow_window_timeout_set(int timeout) {
	window_timeout = timeout;
}
