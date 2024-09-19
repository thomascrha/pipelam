#include <gtk/gtk.h>

#include "config.h"
#include "gtk4-layer-shell.h"
#include "log.h"

static gboolean *bow_get_anchor(enum bow_window_anchor anchor) {
    gboolean *anchors = (gboolean *)malloc(4 * sizeof(gboolean)); // Allocate memory for the array
    if (anchors == NULL) {
        bow_log_error("Memory allocation failed");
        return NULL;
    }

    anchors[0] = FALSE;
    anchors[1] = FALSE;
    anchors[2] = FALSE;
    anchors[3] = FALSE; // Initialize with default values

    if (anchor == BOTTOM_LEFT) {
        anchors[0] = TRUE;
        anchors[3] = TRUE; // BOTTOM LEFT
    } else if (anchor == BOTTOM_RIGHT) {
        anchors[1] = TRUE;
        anchors[3] = TRUE; // BOTTOM RIGHT
    } else if (anchor == TOP_LEFT) {
        anchors[0] = TRUE;
        anchors[2] = TRUE; // TOP LEFT
    } else if (anchor == TOP_RIGHT) {
        anchors[1] = TRUE;
        anchors[2] = TRUE; // TOP RIGHT
    } else if (anchor == CENTER) {
        return anchors; // All values are already FALSE for CENTER
    } else {
        bow_log_error("Unknown anchor: %d, using center", anchor);
        return anchors; // Return default values
    }

    return anchors;
}

static void bow_render_window(GtkApplication *app, gpointer data) {
    bow_log_info("Creating window");
    bow_log_info("Received string: %s", ((struct bow_config *)data)->volume_expression);

    if (((struct bow_config *)data)->volume_expression == NULL) {
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

    // Set margins
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_LEFT, ((struct bow_config *)data)->margin_left);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_RIGHT, ((struct bow_config *)data)->margin_right);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_TOP, ((struct bow_config *)data)->margin_top);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_BOTTOM, ((struct bow_config *)data)->margin_bottom);

    // Set anchors
    gboolean *anchors = bow_get_anchor(((struct bow_config *)data)->anchor);
    for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
        gtk_layer_set_anchor(gtk_window, i, anchors[i]);
    }
    free(anchors);

    // Set up a widget
    GtkWidget *label = gtk_label_new(NULL);
    if (label == NULL) {
        bow_log_error("gtk_label_new() returned NULL");
        return;
    }

    // print len of volume_expression
    bow_log_debug("len of volume_expression: %lu", strlen(((struct bow_config *)data)->volume_expression));

    // combine markup and border
    gtk_label_set_markup(GTK_LABEL(label), ((struct bow_config *)data)->volume_expression);
    gtk_window_set_child(gtk_window, label);

    gtk_window_present(gtk_window);

    bow_log_debug("window_timeout: %d", ((struct bow_config *)data)->window_timeout);
    g_timeout_add(((struct bow_config *)data)->window_timeout, (GSourceFunc)gtk_window_close, gtk_window);
}

int bow_create_run_window(char *volume_expression, int window_timeout) {
    bow_log_info("Received string: %s", volume_expression);
    GtkApplication *app = gtk_application_new("com.github.wmww.bow", G_APPLICATION_DEFAULT_FLAGS);
    gpointer data = g_new(struct bow_config, 1);
    ((struct bow_config *)data)->volume_expression = volume_expression;
    ((struct bow_config *)data)->window_timeout = window_timeout;
    g_signal_connect(app, "activate", G_CALLBACK(bow_render_window), data);
    int status = g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
    g_free(data);
    return status;
}
