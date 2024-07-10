#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

#include "window.h"
#include "log.h"

#define BUFFER_SIZE 2048

int main(int argc, char *argv[]) {
	bow_log_info("Starting bow");

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

		char volume_expression[BUFFER_SIZE];
		if (fgets(volume_expression, BUFFER_SIZE, pipe_fd) == NULL) {
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
