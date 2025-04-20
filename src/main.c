#include "cli.h"
#include "log.h"
#include "message.h"
#include "window.h"
#include <fcntl.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <sys/stat.h>

// I don't like this but GTK complains if i don't do this
static void on_app_activate(void) {
    // This is a minimal handler for the activate signal
    // We don't need to do anything here since we manage windows separately
    pipelam_log_debug("Application activated");
}

static gboolean pipelam_handle_pipe_input(GIOChannel *source, GIOCondition condition G_GNUC_UNUSED, gpointer ptr_pipelam_config) {
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

        pipelam_parse_message(message, ptr_pipelam_config);
        pipelam_create_window(ptr_pipelam_config);
        pipelam_reset_default_config(ptr_pipelam_config);

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
    if (optind >= argc) {
        pipelam_log_error("Please provide a FIFO pipe");
        pipelam_help();
        return EXIT_FAILURE;
    }

    char *pipe_path = argv[optind];

    if (access(pipe_path, F_OK) == -1) {
        mkfifo(pipe_path, 0666);
    }

    int pipe_fd = open(pipe_path, O_RDONLY | O_NONBLOCK);
    if (pipe_fd == -1) {
        perror("open");
        pipelam_log_panic("Failed to open pipe in non-blocking mode - exiting");
        pipelam_destroy_config(pipelam_config);
        return EXIT_FAILURE;
    }

    GtkApplication *pipelam_app = gtk_application_new("com.github.thomascrha.pipelam", G_APPLICATION_NON_UNIQUE);
    g_signal_connect(pipelam_app, "activate", G_CALLBACK(on_app_activate), NULL);
    g_application_register(G_APPLICATION(pipelam_app), NULL, NULL);
    g_application_activate(G_APPLICATION(pipelam_app));
    pipelam_set_application(pipelam_app);

    GMainLoop *main_loop = g_main_loop_new(NULL, FALSE);
    GIOChannel *io_channel = g_io_channel_unix_new(pipe_fd);
    g_io_channel_set_encoding(io_channel, NULL, NULL); // Binary mode
    g_io_channel_set_flags(io_channel, G_IO_FLAG_NONBLOCK, NULL);

    g_io_add_watch(io_channel, G_IO_IN, pipelam_handle_pipe_input, pipelam_config);

    pipelam_log_info("Starting pipelam with log level %s", pipelam_config->log_level);
    pipelam_log_info("Pipe set up in non-blocking mode, waiting for messages on: %s", pipe_path);
    g_main_loop_run(main_loop);

    g_io_channel_unref(io_channel);
    close(pipe_fd);
    g_main_loop_unref(main_loop);
    g_object_unref(pipelam_app);
    pipelam_destroy_config(pipelam_config);

    return EXIT_SUCCESS;
}
