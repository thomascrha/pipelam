#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

#include "gtk4-layer-shell.h"
#include <gtk/gtk.h>

static int display_value = 0;

static void ativate(GtkApplication* app) {
	// Create a normal GTK window however you like
	GtkWindow *gtk_window = GTK_WINDOW(gtk_application_window_new (app));

	// Before the window is first realized, set it up to be a layer surface
	gtk_layer_init_for_window(gtk_window);

	// Order below normal windows
	gtk_layer_set_layer(gtk_window, GTK_LAYER_SHELL_LAYER_TOP);

	// Push other windows out of the way
	gtk_layer_auto_exclusive_zone_enable(gtk_window);

	// We don't need to get keyboard input
	// gtk_layer_set_keyboard_mode (gtk_window, GTK_LAYER_SHELL_KEYBOARD_MODE_NONE); // NONE is default

	// The margins are the gaps around the window's edges
	// Margins and anchors can be set like this...
	gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_LEFT, 40);
	gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_RIGHT, 40);
	gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_TOP, 20);
	gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_BOTTOM, 0); // 0 is default

	// ... or like this
	// Anchors are if the window is pinned to each edge of the output
	static const gboolean anchors[] = {FALSE, FALSE, FALSE, FALSE};
	for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
		gtk_layer_set_anchor(gtk_window, i, anchors[i]);
	}

	// Set up a widget
	GtkWidget *label = gtk_label_new("");

	char bla[1024];   // Use an array which is large enough
	snprintf(bla, sizeof(bla), "<span font_desc=\"100.0\">%d</span>", display_value);

	gtk_label_set_markup(GTK_LABEL(label), bla);
	gtk_window_set_child(gtk_window, label);

	gtk_window_present(gtk_window);

	// wait for 600 milliseconds and then close the window
	g_timeout_add(600, (GSourceFunc)gtk_window_close, gtk_window);

}

int window() {
	GtkApplication *app = gtk_application_new("com.github.wmww.gtk4-layer-shell.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(ativate), NULL);
	int status = g_application_run(G_APPLICATION(app), 0, NULL);
	printf("Exiting with status %d\n", status);
	g_object_unref(app);
	return status;
}

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

		if (integer >= 0 && integer <= 100) {
			display_value = integer;
			int code = window();
			if (code != 0) {
				fprintf(stderr, "ERROR: window() returned %d\n", code);
				return EXIT_FAILURE;
			}

		};
        printf("Received integer: %d\n", integer);
    }

    return EXIT_SUCCESS;
}
