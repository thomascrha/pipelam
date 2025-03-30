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
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static gboolean handle_pipe_input(GIOChannel *source, GIOCondition condition G_GNUC_UNUSED, gpointer user_data) {
    struct pipelam_config *config = (struct pipelam_config *)user_data;
    gchar *message = NULL;
    gsize length;
    GError *error = NULL;
    GIOStatus status;

    // Read a line from the pipe
    status = g_io_channel_read_line(source, &message, &length, NULL, &error);

    if (status == G_IO_STATUS_ERROR) {
        pipelam_log_error("Error reading from pipe: %s", error->message);
        g_error_free(error);
        return TRUE; // Continue watching
    }

    if (status == G_IO_STATUS_EOF) {
        pipelam_log_debug("End of file reached, reopening pipe");
        return TRUE; // Continue watching
    }

    if (status == G_IO_STATUS_AGAIN) {
        // No data available, but we keep watching
        return TRUE;
    }

    if (message != NULL && length > 0) {
        pipelam_log_debug("Received message of length: %lu", length);

        // If there's an active window, close it first to ensure new message shows immediately
        if (pipelam_has_active_window()) {
            pipelam_log_debug("Active window detected, closing it");
            pipelam_close_current_window();
        }

        // Process the message
        pipelam_parse_message(message, config);

        // Create and run window with the new message
        pipelam_create_run_window(config);

        g_free(message);
    }

    // We want to keep being notified
    return TRUE;
}

int main(int argc, char *argv[]) {
    struct pipelam_config *pipelam_config = pipelam_setup_config(NULL);
    if (pipelam_config == NULL) {
        pipelam_log_panic("Failed to setup bow config - exiting");
        pipelam_destroy_config(pipelam_config);
        return EXIT_FAILURE;
    }

    pipelam_log_info("Starting bow with log level %s", pipelam_config->log_level);

    if (argc != 2) {
        pipelam_log_panic("Usage: %s <pipe_path>", argv[0]);
    };

    char *pipe_path = argv[1];

    if (access(pipe_path, F_OK) == -1) {
        mkfifo(pipe_path, 0666);
    }

    // Set up GLib main loop for event handling
    GMainLoop *main_loop = g_main_loop_new(NULL, FALSE);

    // Set up a file descriptor for the pipe
    int pipe_fd = open(pipe_path, O_RDONLY | O_NONBLOCK);
    if (pipe_fd == -1) {
        perror("open");
        pipelam_log_panic("Failed to open pipe in non-blocking mode - exiting");
        pipelam_destroy_config(pipelam_config);
        return EXIT_FAILURE;
    }

    // Create a GIOChannel from the file descriptor
    GIOChannel *io_channel = g_io_channel_unix_new(pipe_fd);
    g_io_channel_set_encoding(io_channel, NULL, NULL); // Binary mode
    g_io_channel_set_flags(io_channel, G_IO_FLAG_NONBLOCK, NULL);

    // Set up a watch for the channel
    guint watch_id = g_io_add_watch(io_channel, G_IO_IN, handle_pipe_input, pipelam_config);

    pipelam_log_info("Pipe set up in non-blocking mode, waiting for messages on: %s", pipe_path);

    // Start the main loop
    g_main_loop_run(main_loop);

    // Clean up (this code is reached only if the main loop is quit)
    g_source_remove(watch_id);
    g_io_channel_unref(io_channel);
    close(pipe_fd);
    g_main_loop_unref(main_loop);

    pipelam_destroy_config(pipelam_config);
    return EXIT_SUCCESS;
}
