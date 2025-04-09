#include "log.h"
#include "message.h"
#include "window.h"
#include <fcntl.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <sys/stat.h>

static GtkApplication *pipelam_app = NULL;

#define MAX_QUEUE_SIZE 50
static char *message_queue[MAX_QUEUE_SIZE] = {NULL};
static int queue_head = 0;
static int queue_tail = 0;
static int queue_count = 0;
static int message_process_timer_id = 0;
static bool processing_message = FALSE;

static void on_app_activate(void) { pipelam_log_debug("Application activated"); }

static gboolean pipelam_process_message_from_queue(gpointer ptr_pipelam_config) {
    if (processing_message || queue_count == 0) {
        return G_SOURCE_CONTINUE;
    }
    processing_message = TRUE;

    struct pipelam_config *config = (struct pipelam_config *)ptr_pipelam_config;
    char *message = NULL;

    if (queue_count > 1) {
        pipelam_log_debug("Multiple messages in queue (%d), using the latest", queue_count);
        int latest_idx = (queue_tail + MAX_QUEUE_SIZE - 1) % MAX_QUEUE_SIZE;

        if (message_queue[latest_idx] != NULL) {
            message = g_strdup(message_queue[latest_idx]);
            pipelam_log_debug("Selected latest message: %s", message);
            for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
                if (message_queue[i] != NULL) {
                    g_free(message_queue[i]);
                    message_queue[i] = NULL;
                }
            }

            queue_head = 0;
            queue_tail = 0;
            queue_count = 0;
        }
    } else {
        message = message_queue[queue_head];
        message_queue[queue_head] = NULL;
        queue_head = (queue_head + 1) % MAX_QUEUE_SIZE;
        queue_count--;
    }

    if (message != NULL) {
        pipelam_log_debug("Processing message: %s", message);
        pipelam_parse_message(message, config);
        pipelam_set_application(pipelam_app);
        pipelam_create_window(config);
        pipelam_reset_default_config(config);
        g_free(message);
    } else {
        pipelam_log_error("No message found to process even though queue_count > 0");
    }

    processing_message = FALSE;
    return G_SOURCE_CONTINUE;
}

static gboolean pipelam_handle_pipe_input(GIOChannel *source, GIOCondition condition G_GNUC_UNUSED, gpointer ptr_pipelam_config) {
    char *message = NULL;
    size_t length;
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

        if (queue_count >= MAX_QUEUE_SIZE) {
            pipelam_log_warning("Message queue full, dropping message");
            g_free(message);
            return TRUE;
        }

        // Add message to queue
        message_queue[queue_tail] = message; // Transfer ownership of message
        queue_tail = (queue_tail + 1) % MAX_QUEUE_SIZE;
        queue_count++;

        // Start the message processing timer if not already running
        if (message_process_timer_id == 0) {
            message_process_timer_id = g_timeout_add(100, pipelam_process_message_from_queue, ptr_pipelam_config);
        }
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
        return EXIT_FAILURE;
    }

    pipelam_app = gtk_application_new("com.github.thomascrha.pipelam", G_APPLICATION_NON_UNIQUE);
    g_signal_connect(pipelam_app, "activate", G_CALLBACK(on_app_activate), NULL);
    g_application_register(G_APPLICATION(pipelam_app), NULL, NULL);
    g_application_activate(G_APPLICATION(pipelam_app));

    pipelam_log_info("Starting pipelam with log level %s", pipelam_config->log_level);
    GMainLoop *main_loop = g_main_loop_new(NULL, FALSE);
    GIOChannel *io_channel = g_io_channel_unix_new(pipe_fd);

    g_io_add_watch(io_channel, G_IO_IN, pipelam_handle_pipe_input, pipelam_config);

    pipelam_log_info("Pipe set up in non-blocking mode, waiting for messages on: %s", pipe_path);

    g_main_loop_run(main_loop);

    return EXIT_SUCCESS;
}
