#include "log.h"
#include "message.h"
#include "window.h"
#include <fcntl.h>
#include <gtk/gtk.h>
#include <sys/stat.h>

static gboolean handle_pipe_input(GIOChannel *source, GIOCondition condition G_GNUC_UNUSED, gpointer user_data) {
    struct pipelam_config *config = (struct pipelam_config *)user_data;
    gchar *message = NULL;
    gsize length;
    GError *error = NULL;
    GIOStatus status;

    status = g_io_channel_read_line(source, &message, &length, NULL, &error);

    if (status == G_IO_STATUS_ERROR) {
        pipelam_log_error("Error reading from pipe: %s", error->message);
        g_error_free(error);
        return TRUE;
    }

    if (status == G_IO_STATUS_EOF) {
        pipelam_log_debug("End of file reached, reopening pipe");
        return TRUE;
    }

    if (status == G_IO_STATUS_AGAIN) {
        return TRUE;
    }

    if (message != NULL && length > 0) {
        pipelam_log_info("Received message of length: %lu", length);

        pipelam_log_info("ruintime %d", config->runtime_behaviour);
        if (config->runtime_behaviour == (int)REPLACE) {
            pipelam_log_debug("Runtime behavior is REPLACE, closing any existing window");
            pipelam_close_current_window();
        }

        pipelam_parse_message(message, config);
        pipelam_create_window(config);
        pipelam_reset_default_config(config);
        g_free(message);
    }

    return TRUE;
}

int main(int argc, char *argv[]) {
    struct pipelam_config *pipelam_config = pipelam_setup_config(NULL);
    if (pipelam_config == NULL) {
        pipelam_log_panic("Failed to setup pipelam config - exiting");
        pipelam_destroy_config(pipelam_config);
        return EXIT_FAILURE;
    }

    // Process command line arguments - these always override
    // the config file and environment variables
    pipelam_process_command_line_args(argc, argv, pipelam_config);

    // Check if pipe path is provided (should be the last argument)
    if (optind >= argc) {
        pipelam_log_panic("Usage: %s [OPTIONS] <pipe_path>", argv[0]);
    }

    gtk_init();

    pipelam_log_info("Starting pipelam with log level %s", pipelam_config->log_level);

    char *pipe_path = argv[optind];

    if (access(pipe_path, F_OK) == -1) {
        mkfifo(pipe_path, 0666);
    }

    GMainLoop *main_loop = g_main_loop_new(NULL, FALSE);
    int pipe_fd = open(pipe_path, O_RDONLY | O_NONBLOCK);
    if (pipe_fd == -1) {
        perror("open");
        pipelam_log_panic("Failed to open pipe in non-blocking mode - exiting");
        pipelam_destroy_config(pipelam_config);
        return EXIT_FAILURE;
    }

    GIOChannel *io_channel = g_io_channel_unix_new(pipe_fd);
    g_io_channel_set_encoding(io_channel, NULL, NULL); // Binary mode
    g_io_channel_set_flags(io_channel, G_IO_FLAG_NONBLOCK, NULL);

    guint watch_id = g_io_add_watch(io_channel, G_IO_IN, handle_pipe_input, pipelam_config);

    pipelam_log_info("Pipe set up in non-blocking mode, waiting for messages on: %s", pipe_path);

    g_main_loop_run(main_loop);

    // Clean up (this code is reached only if the main loop is quit)
    g_source_remove(watch_id);
    g_io_channel_unref(io_channel);
    close(pipe_fd);
    g_main_loop_unref(main_loop);
    pipelam_destroy_config(pipelam_config);

    return EXIT_SUCCESS;
}
