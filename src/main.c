#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

#include "window.h"

#define BUFFER_SIZE 2048

int main(int argc, char *argv[]) {

	if (argc != 2) {
		fprintf(stderr, "ERROR: Usage: %s <pipe_path>\n", argv[0]);
		return EXIT_FAILURE;
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

		char str[BUFFER_SIZE];
		if (fgets(str, BUFFER_SIZE, pipe_fd) == NULL) {
			fprintf(stderr, "ERROR: Input is larger than buffer size\n");
			continue;
		}
		fclose(pipe_fd);

		fprintf(stdout, "INFO: Received string: %s\n", str);

		// int code = bow_create_run_window();
		// if (code != 0) {
		// 	fprintf(stderr, "ERROR: bow_create_run_window() returned %d\n", code);
		// 	return EXIT_FAILURE;
		// }

	}

	return EXIT_SUCCESS;
}
