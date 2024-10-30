#include <gtk-4.0/gtk/gtk.h>
#include <gtk/gtk.h>

#include "config.h"
#include "gdk-pixbuf/gdk-pixbuf.h"
#include "gdk/gdk.h"
#include "gio/gio.h"
#include "glib.h"
#include "gtk/gtkshortcut.h"
#include "gtk4-layer-shell.h"
#include "log.h"

static gboolean *bow_get_anchor(enum bow_window_anchor anchor) {
    gboolean *anchors = (gboolean *)malloc(4 * sizeof(gboolean));

    if (anchors == NULL) {
        bow_log_error("Memory allocation failed");
        return NULL;
    }

    anchors[0] = FALSE;
    anchors[1] = FALSE;
    anchors[2] = FALSE;
    anchors[3] = FALSE; // Initialize with default values

    bow_log_debug("current anchor: %d", anchor);

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

static GtkWindow *bow_render_gtk_window(GtkApplication *app, gpointer bow_config) {
    bow_log_info("Creating window");
    bow_log_info("Received string: %s", ((struct bow_config *)bow_config)->expression);

    if (((struct bow_config *)bow_config)->expression == NULL) {
        bow_log_error("data is NULL");
        return NULL;
    }

    GtkWindow *gtk_window = GTK_WINDOW(gtk_application_window_new(app));
    if (gtk_window == NULL) {
        bow_log_error("gtk_application_window_new() returned NULL");
        return NULL;
    }

    // Before the window is first realized, set it up to be a layer surface
    gtk_layer_init_for_window(gtk_window);

    // Order below normal windows
    gtk_layer_set_layer(gtk_window, GTK_LAYER_SHELL_LAYER_TOP);

    // Push other windows out of the way
    gtk_layer_auto_exclusive_zone_enable(gtk_window);

    // Set margins
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_LEFT, ((struct bow_config *)bow_config)->margin_left);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_RIGHT, ((struct bow_config *)bow_config)->margin_right);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_TOP, ((struct bow_config *)bow_config)->margin_top);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_BOTTOM, ((struct bow_config *)bow_config)->margin_bottom);

    // Set anchors
    gboolean *anchors = bow_get_anchor(((struct bow_config *)bow_config)->anchor);
    // current anchor print out value of enum
    // bow_log_debug("current anchor: %d", data->anchor);
    for (int i = 0; i < 4; i++) {
        bow_log_debug("anchors[%d]: %d", i, anchors[i]);
        gtk_layer_set_anchor(gtk_window, i, anchors[i]);
    }

    return gtk_window;
}

static void bow_render_image_window(GtkApplication *app, gpointer bow_config) {
    GtkWindow *gtk_window = bow_render_gtk_window(app, bow_config);
    if (gtk_window == NULL) {
        bow_log_error("gtk_render_gtk_window() returned NULL");
        return;
    }
    //
    // GdkPixbuf *_pixbuf = gdk_pixbuf_new_from_file(((struct bow_config *)bow_config)->expression, NULL);
    // if (_pixbuf == NULL) {
    //     bow_log_error("gdk_pixbuf_new_from_stream() returned NULL");
    //     return;
    // }
    //
    // gint width = gdk_pixbuf_get_width(_pixbuf);
    // gint height = gdk_pixbuf_get_height(_pixbuf);
    // bow_log_debug("width: %d, height: %d", width, height);
    //
    // GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_size(((struct bow_config *)bow_config)->expression, width, height, NULL);
    //
    // GdkTexture *texture = gdk_texture_new_for_pixbuf(pixbuf);
    // GdkPaintable *paintable = GDK_PAINTABLE(texture);
    GtkWidget *image = gtk_image_new_from_file(((struct bow_config *)bow_config)->expression);
    // GtkImage *gtk_image = GTK_IMAGE(image);

    if (image == NULL) {
        bow_log_error("gtk_image_new_from_file() returned NULL");
        return;
    }

    gtk_window_set_child(gtk_window, image);
    gtk_window_present(gtk_window);

    bow_log_debug("window_timeout: %d", ((struct bow_config *)bow_config)->window_timeout);
    g_timeout_add(((struct bow_config *)bow_config)->window_timeout, (GSourceFunc)gtk_window_close, gtk_window);
    gtk_window_present(gtk_window);

    g_signal_connect(gtk_window, "destroy", G_CALLBACK(gtk_window_close), NULL);
}

static void bow_render_text_window(GtkApplication *app, gpointer bow_config) {
    GtkWindow *gtk_window = bow_render_gtk_window(app, bow_config);
    if (gtk_window == NULL) {
        bow_log_error("gtk_render_gtk_window() returned NULL");
        return;
    }

    GtkWidget *label = gtk_label_new(NULL);
    if (label == NULL) {
        bow_log_error("gtk_label_new() returned NULL");
        return;
    }

    // print len of expression
    bow_log_info("len of expression: %lu", strlen(((struct bow_config *)bow_config)->expression));
    bow_log_info("expression: %s", ((struct bow_config *)bow_config)->expression);

    // combine markup and border
    gtk_label_set_markup(GTK_LABEL(label), ((struct bow_config *)bow_config)->expression);
    gtk_window_set_child(gtk_window, label);

    bow_log_debug("window_timeout: %d", ((struct bow_config *)bow_config)->window_timeout);
    g_timeout_add(((struct bow_config *)bow_config)->window_timeout, (GSourceFunc)gtk_window_close, gtk_window);
    gtk_window_present(gtk_window);

    g_signal_connect(gtk_window, "destroy", G_CALLBACK(gtk_window_close), NULL);
}

void bow_create_run_window(gpointer bow_config) {
    // bow_log_info("Received string: %s", expression);
    GtkApplication *app = gtk_application_new("com.github.wmww.bow", G_APPLICATION_DEFAULT_FLAGS);

    if (((struct bow_config *)bow_config)->type == IMAGE) {
        g_signal_connect(app, "activate", G_CALLBACK(bow_render_image_window), bow_config);
    } else if (((struct bow_config *)bow_config)->type == TEXT) {
        g_signal_connect(app, "activate", G_CALLBACK(bow_render_text_window), bow_config);
    } else {
        bow_log_error("Unknown type: %d", ((struct bow_config *)bow_config)->type);
        return;
    }
    g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
}
