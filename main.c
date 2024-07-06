#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pipe_path>\n", argv[0]);
        return EXIT_FAILURE;
    };

    char *pipe_path = argv[1];

    if (access(pipe_path, F_OK) == 1) {
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

        if (integer == 0) {
            break;
        }

        printf("Received integer: %d\n", integer);
    }

    // print the integer to the console


    printf("Hello, World!\n");
    return EXIT_SUCCESS;
}
