#include "glib.h"
#include "gtk4-layer-shell.h"
#include <gtk/gtk.h>


static void bow_render_window(GtkApplication* app, char *volume_expression) {
	// Create a normal GTK window however you like
	GtkWindow *gtk_window = GTK_WINDOW(gtk_application_window_new (app));

	// Before the window is first realized, set it up to be a layer surface
	gtk_layer_init_for_window(gtk_window);

	// Order below normal windows
	gtk_layer_set_layer(gtk_window, GTK_LAYER_SHELL_LAYER_TOP);

	// Push other windows out of the way
	gtk_layer_auto_exclusive_zone_enable(gtk_window);

	// Set up a widget
	GtkWidget *label = gtk_label_new(NULL);

	// print len of volume_expression
	printf("len of volume_expression: %lu\n", strlen(volume_expression));


	// combine markup and border
	gtk_label_set_markup(GTK_LABEL(label), volume_expression);
	gtk_window_set_child(gtk_window, label);

	gtk_window_present(gtk_window);

	// wait for 600 milliseconds and then close the window
	g_timeout_add_seconds(2, (GSourceFunc)gtk_window_close, gtk_window);

}

int bow_create_run_window(char *volume_expression) {
	GtkApplication *app = gtk_application_new("com.github.wmww.gtk4-layer-shell.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(bow_render_window), NULL);
	int status = g_application_run(G_APPLICATION(app), *volume_expression, NULL);
	printf("Exiting with status %d\n", status);
	g_object_unref(app);
	return status;
}
