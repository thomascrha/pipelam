#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config.h"
#include "glib.h"
#include "log.h"
#include "message.h"
#include "window.h"

int main(int argc, char *argv[]) {
    gpointer pipelam_config = pipelam_setup_config();
    if (pipelam_config == NULL) {
        pipelam_log_error("Failed to setup bow config");
        pipelam_destroy_config(pipelam_config);
        return EXIT_FAILURE;
    }

    pipelam_log_info("Starting bow with log level %d buffer size %d", ((struct pipelam_config *)pipelam_config)->log_level, ((struct pipelam_config *)pipelam_config)->buffer_size);

    if (argc != 2) {
        pipelam_log_panic("Usage: %s <pipe_path>", argv[0]);
    };

    char *pipe_path = argv[1];

    if (access(pipe_path, F_OK) == -1) {
        mkfifo(pipe_path, 0666);
    }

    while (1) {
        // If there's an active window, close it first to ensure new message shows immediately
        if (pipelam_has_active_window()) {
            pipelam_close_current_window();
        }

        FILE *pipe_fd = fopen(pipe_path, "r");
        if (pipe_fd == NULL) {
            perror("fopen");
            pipelam_log_error("Failed to open pipe");
            pipelam_destroy_config(pipelam_config);
            return EXIT_FAILURE;
        }

        char volume_expression[((struct pipelam_config *)pipelam_config)->buffer_size];
        if (fgets(volume_expression, ((struct pipelam_config *)pipelam_config)->buffer_size, pipe_fd) == NULL) {
            pipelam_log_error("Input is larger than buffer size");
            continue;
        }
        fclose(pipe_fd);

        pipelam_log_info("Received string");
        pipelam_log_debug("%s", volume_expression);
        pipelam_parse_message(volume_expression, pipelam_config);

        pipelam_create_run_window(pipelam_config);
    }

    pipelam_destroy_config(pipelam_config);
    return EXIT_SUCCESS;
}
