#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "ERROR: Usage: %s <pipe_path>\n", argv[0]);
        return EXIT_FAILURE;
    };

    char *pipe_path = argv[1];

    fprintf(stdout, "INFO: pipe_path: %s\n", access(pipe_path, F_OK) ? "does not exist" : "exists");

    if (access(pipe_path, F_OK) == false) {
        // create a named pipe if it does not exist
        mkfifo(pipe_path, 0666);
    }

    // wait for a integer to writen to the pipe file descriptor
    while (1) {
        FILE *pipe_fd = fopen(pipe_path, "r");
        if (pipe_fd == NULL) {
            perror("fopen");
            return EXIT_FAILURE;
        }

        int integer;
        fscanf(pipe_fd, "%d", &integer);
        fclose(pipe_fd);

        printf("Received integer: %d\n", integer);
    }

    return EXIT_SUCCESS;
}
