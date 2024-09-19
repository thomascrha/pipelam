#include <gtk/gtk.h>

#include "gtk4-layer-shell.h"
#include "log.h"

struct data {
    char *volume_expression;
    int window_timeout;
};

static void bow_render_window(GtkApplication *app, gpointer data) {
    bow_log_info("Creating window");
    bow_log_info("Received string: %s",
                 ((struct data *)data)->volume_expression);

    if (((struct data *)data)->volume_expression == NULL) {
        bow_log_error("data is NULL");
        return;
    }

    GtkWindow *gtk_window = GTK_WINDOW(gtk_application_window_new(app));
    if (gtk_window == NULL) {
        bow_log_error("gtk_application_window_new() returned NULL");
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
        bow_log_error("gtk_label_new() returned NULL");
        return;
    }

    // print len of volume_expression
    bow_log_debug("len of volume_expression: %lu",
                  strlen(((struct data *)data)->volume_expression));

    // combine markup and border
    gtk_label_set_markup(GTK_LABEL(label),
                         ((struct data *)data)->volume_expression);
    gtk_window_set_child(gtk_window, label);

    gtk_window_present(gtk_window);

    bow_log_debug("window_timeout: %d", ((struct data *)data)->window_timeout);
    g_timeout_add(((struct data *)data)->window_timeout,
                  (GSourceFunc)gtk_window_close, gtk_window);
}

int bow_create_run_window(char *volume_expression, int window_timeout) {
    bow_log_info("Received string: %s", volume_expression);
    GtkApplication *app =
        gtk_application_new("com.github.wmww.bow", G_APPLICATION_DEFAULT_FLAGS);
    gpointer data = g_new(struct data, 1);
    ((struct data *)data)->volume_expression = volume_expression;
    ((struct data *)data)->window_timeout = window_timeout;
    g_signal_connect(app, "activate", G_CALLBACK(bow_render_window), data);
    int status = g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
    g_free(data);
    return status;
}
