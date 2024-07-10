#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

#include "window.h"
#include "log.h"
#include "config.h"


int main(int argc, char *argv[]) {
	const char *log_level = getenv("BOW_LOG_LEVEL");
	if (log_level == NULL) {
		log_level = "INFO";
	}

	const char *buffer_size_env = getenv("BOW_BUFFER_SIZE");
	if (buffer_size_env != NULL) {
		printf("buffer_size_env: %s\n", buffer_size_env);
		buffer_size = atoi(buffer_size_env);
	}

	bow_log_level_set_from_string(log_level);
	bow_log_info("Starting bow with log level %s buffer size %d", log_level, buffer_size);

	if (argc != 2) {
		bow_log_panic("Usage: %s <pipe_path>", argv[0]);
	};

	char *pipe_path = argv[1];

	if (access(pipe_path, F_OK) == -1) {
		mkfifo(pipe_path, 0666);
	}

	while (1) {
		FILE *pipe_fd = fopen(pipe_path, "r");
		if (pipe_fd == NULL) {
			perror("fopen");
			return EXIT_FAILURE;
		}

		char volume_expression[buffer_size];
		if (fgets(volume_expression, buffer_size, pipe_fd) == NULL) {
			bow_log_error("Input is larger than buffer size");
			continue;
		}
		fclose(pipe_fd);

		bow_log_info("Received string: %s", volume_expression);

		int code = bow_create_run_window(volume_expression);
		if (code != 0) {
			bow_log_panic("bow_create_run_window() returned %d", code);
			return EXIT_FAILURE;
		}

	}

	return EXIT_SUCCESS;
}
