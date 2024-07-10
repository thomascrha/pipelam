#include "glib.h"
#include "gtk4-layer-shell.h"
#include <gtk/gtk.h>

struct data {
	GtkApplication *app;
	char *volume_expression;
};

// static void bow_render_window(GtkApplication* app, char *volume_expression) {
static void bow_render_window(struct data *data) {
	// Create a normal GTK window however you like
	fprintf(stdout, "DEBUG: Creating window\n");
	fprintf(stdout, "DEBUG: data->volume_expression: %s\n", data->volume_expression);
	GtkWindow *gtk_window = GTK_WINDOW(gtk_application_window_new (data->app));
	if (gtk_window == NULL) {
		fprintf(stderr, "ERROR: gtk_application_window_new() returned NULL\n");
		return;
	}

	// Before the window is first realized, set it up to be a layer surface
	gtk_layer_init_for_window(gtk_window);

	// Order below normal windows
	gtk_layer_set_layer(gtk_window, GTK_LAYER_SHELL_LAYER_TOP);

	// Push other windows out of the way
	gtk_layer_auto_exclusive_zone_enable(gtk_window);

	// Set up a widget
	GtkWidget *label = gtk_label_new(NULL);
	if (label == NULL) {
		fprintf(stderr, "ERROR: gtk_label_new() returned NULL\n");
		return;
	}

	// print len of volume_expression

	printf("len of volume_expression: %lu\n", strlen(data->volume_expression));
	// combine markup and border
	gtk_label_set_markup(GTK_LABEL(label), data->volume_expression);
	gtk_window_set_child(gtk_window, label);

	gtk_window_present(gtk_window);

	// wait for 600 milliseconds and then close the window
	g_timeout_add_seconds(2, (GSourceFunc)gtk_window_close, gtk_window);

}

int bow_create_run_window(char *volume_expression) {
	fprintf(stdout, "INFO: Received string: %s\n", volume_expression);
    GtkApplication *app = gtk_application_new("com.github.wmww.bow", G_APPLICATION_FLAGS_NONE);
    struct data *data = g_new(struct data, 1);
    data->app = app;
    data->volume_expression = volume_expression;
    g_signal_connect_data(app, "activate", G_CALLBACK(bow_render_window), data, (GClosureNotify)free, 0);
    return g_application_run(G_APPLICATION(app), 0, NULL);
}

