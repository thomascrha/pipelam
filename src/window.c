#include <gtk/gtk.h>

#include "gtk4-layer-shell.h"
#include "log.h"

struct data {
    char *expression;
    int window_timeout;
};

static void bow_render_window(GtkApplication *app, gpointer _data) {
    bow_log_debug("bow_render_image_window() called");
    struct data *data = (struct data *)_data;

    GtkWindow *gtk_window = GTK_WINDOW(gtk_application_window_new(app));
    if (gtk_window == NULL) {
        bow_log_error("gtk_application_window_new() returned NULL");
        return;
    }

    gtk_layer_init_for_window(gtk_window);
    gtk_layer_set_layer(gtk_window, GTK_LAYER_SHELL_LAYER_TOP);
    gtk_layer_auto_exclusive_zone_enable(gtk_window);

    char *path_expression = data->expression;
    // replace the last character with a null terminator as it will be a
    // newline because of fgets and how it works
    path_expression = g_strchomp(path_expression);
    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(path_expression, &error);
    // GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("/home/tcra/Projects/bow/examples/vol.png", &error);
    if (pixbuf == NULL) {
        bow_log_error("gdk_pixbuf_new_from_file() returned NULL: %s", error->message);
        g_error_free(error);
        return;
    }

    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);

    bow_log_debug("width: %d, height: %d", width, height);

    GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, width, height, GDK_INTERP_BILINEAR);

    GtkWidget *image = gtk_image_new_from_pixbuf(scaled_pixbuf);
    if (image == NULL) {
        bow_log_error("gtk_image_new_from_pixbuf() returned NULL");
        return;
    }

    gtk_window_set_child(gtk_window, image);
    gtk_window_set_default_size(gtk_window, width, height); // Set the window size
    gtk_window_present(gtk_window);

    bow_log_debug("window_timeout: %d", data->window_timeout);
    g_timeout_add(data->window_timeout, (GSourceFunc)gtk_window_close, gtk_window);
}
//
// static void bow_render_window(GtkApplication *app, gpointer _data) {
//     bow_log_debug("bow_render_window() called");
//     struct data *data = (struct data *)_data;
//
//     GtkWindow *gtk_window = GTK_WINDOW(gtk_application_window_new(app));
//     if (gtk_window == NULL) {
//         bow_log_error("gtk_application_window_new() returned NULL");
//         return;
//     }
//
//     gtk_layer_init_for_window(gtk_window);
//     gtk_layer_set_layer(gtk_window, GTK_LAYER_SHELL_LAYER_TOP);
//     gtk_layer_auto_exclusive_zone_enable(gtk_window);
//
//     GtkWidget *label = gtk_label_new(NULL);
//     if (label == NULL) {
//         bow_log_error("gtk_label_new() returned NULL");
//         return;
//     }
//
//     char *volume_expression = data->expression;
//     // replace the last character with a null terminator as it will be a
//     // newline because of fgets and how it works
//     volume_expression = g_strchomp(volume_expression);
//
//     gtk_label_set_markup(GTK_LABEL(label), data->expression);
//     gtk_window_set_child(gtk_window, label);
//     gtk_window_present(gtk_window);
//
//     bow_log_debug("window_timeout: %d", data->window_timeout);
//     g_timeout_add(data->window_timeout, (GSourceFunc)gtk_window_close, gtk_window);
//     g_object_unref(label);
//     g_object_unref(gtk_window);
// }

int bow_create_run_window(char *expression, int window_timeout) {
    bow_log_debug("bow_create_run_window() called");

    // TODO make this a #define global variable in config.h
    GtkApplication *app = gtk_application_new("com.github.thomascrha.bow", G_APPLICATION_FLAGS_NONE);

    gpointer data = g_new(struct data, 1);
    ((struct data *)data)->expression = expression;
    ((struct data *)data)->window_timeout = window_timeout;

    g_signal_connect(app, "activate", G_CALLBACK(bow_render_window), data);

    int status = g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
    g_free(data);

    return status;
}
