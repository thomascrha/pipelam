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
    // Allocate memory for GTK_LAYER_SHELL_EDGE_LEFT, RIGHT, TOP, BOTTOM
    gboolean *anchors = (gboolean *)malloc(4 * sizeof(gboolean));

    if (anchors == NULL) {
        bow_log_error("Memory allocation failed");
        return NULL;
    }

    // Initialize all anchors to FALSE
    // [0]=LEFT, [1]=RIGHT, [2]=TOP, [3]=BOTTOM
    for (int i = 0; i < 4; i++) {
        anchors[i] = FALSE;
    }

    bow_log_debug("current anchor: %d", anchor);

    switch (anchor) {
    case BOTTOM_LEFT:
        anchors[GTK_LAYER_SHELL_EDGE_LEFT] = TRUE;
        anchors[GTK_LAYER_SHELL_EDGE_BOTTOM] = TRUE;
        break;
    case BOTTOM_RIGHT:
        anchors[GTK_LAYER_SHELL_EDGE_RIGHT] = TRUE;
        anchors[GTK_LAYER_SHELL_EDGE_BOTTOM] = TRUE;
        break;
    case TOP_LEFT:
        anchors[GTK_LAYER_SHELL_EDGE_LEFT] = TRUE;
        anchors[GTK_LAYER_SHELL_EDGE_TOP] = TRUE;
        break;
    case TOP_RIGHT:
        anchors[GTK_LAYER_SHELL_EDGE_RIGHT] = TRUE;
        anchors[GTK_LAYER_SHELL_EDGE_TOP] = TRUE;
        break;
    case CENTER:
        // All anchors remain FALSE
        break;
    default:
        bow_log_error("Unknown anchor: %d, using center", anchor);
        // All anchors remain FALSE
        break;
    }

    return anchors;
}

static GtkWindow *bow_render_gtk_window(GtkApplication *app, gpointer bow_config) {
    bow_log_debug("Creating window");
    bow_log_debug("Received string: %s", ((struct bow_config *)bow_config)->expression);

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
    if (anchors != NULL) {
        // Explicitly use the GTK_LAYER_SHELL_EDGE_* constants for clarity
        gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_LEFT, anchors[GTK_LAYER_SHELL_EDGE_LEFT]);
        gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_RIGHT, anchors[GTK_LAYER_SHELL_EDGE_RIGHT]);
        gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_TOP, anchors[GTK_LAYER_SHELL_EDGE_TOP]);
        gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_BOTTOM, anchors[GTK_LAYER_SHELL_EDGE_BOTTOM]);

        // Debug logging
        for (int i = 0; i < 4; i++) {
            bow_log_debug("anchors[%d]: %d", i, anchors[i]);
        }

        free(anchors); // Free memory after use
    }

    return gtk_window;
}

static gboolean close_window_callback(gpointer window) {
    gtk_window_close(GTK_WINDOW(window));
    return G_SOURCE_REMOVE; // Return FALSE to remove the source
}

static void bow_render_image_window(GtkApplication *app, gpointer bow_config) {
    bow_log_debug("Creating image window");
    GtkWindow *gtk_window = bow_render_gtk_window(app, bow_config);
    if (gtk_window == NULL) {
        bow_log_error("gtk_render_gtk_window() returned NULL");
        return;
    }

    const char *image_path = ((struct bow_config *)bow_config)->expression;
    bow_log_debug("Loading image from path: %s", image_path);

    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path, &error);
    if (pixbuf == NULL) {
        bow_log_error("Failed to load image: %s", error ? error->message : "Unknown error");
        if (error)
            g_error_free(error);
        return;
    }

    gint width = gdk_pixbuf_get_width(pixbuf);
    gint height = gdk_pixbuf_get_height(pixbuf);
    bow_log_debug("Image dimensions - width: %d, height: %d", width, height);

    // Create paintable from pixbuf
    GdkPaintable *paintable = GDK_PAINTABLE(gdk_texture_new_for_pixbuf(pixbuf));
    if (paintable == NULL) {
        bow_log_error("Failed to create paintable from pixbuf");
        g_object_unref(pixbuf);
        return;
    }

    // Create image widget from paintable
    GtkWidget *image = gtk_image_new_from_paintable(paintable);
    if (image == NULL) {
        bow_log_error("Failed to create image widget");
        g_object_unref(paintable);
        g_object_unref(pixbuf);
        return;
    }

    // Make sure image is shown at its natural size
    gtk_widget_set_size_request(image, width, height);
    gtk_image_set_pixel_size(GTK_IMAGE(image), -1); // Use natural size

    // Create a box to hold the image with proper sizing
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_append(GTK_BOX(box), image);

    gtk_window_set_child(gtk_window, box);

    // Set window to default size matching the image
    gtk_window_set_default_size(gtk_window, width, height);

    bow_log_debug("window_timeout: %d", ((struct bow_config *)bow_config)->window_timeout);
    g_timeout_add(((struct bow_config *)bow_config)->window_timeout, close_window_callback, gtk_window);
    gtk_window_present(gtk_window);

    g_signal_connect(gtk_window, "destroy", G_CALLBACK(gtk_window_close), NULL);

    // Unref paintable and pixbuf after they're used
    g_object_unref(paintable);
    g_object_unref(pixbuf);
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
    bow_log_debug("len of expression: %lu", strlen(((struct bow_config *)bow_config)->expression));
    bow_log_debug("expression: %s", ((struct bow_config *)bow_config)->expression);

    // combine markup and border
    gtk_label_set_markup(GTK_LABEL(label), ((struct bow_config *)bow_config)->expression);
    gtk_window_set_child(gtk_window, label);

    bow_log_debug("window_timeout: %d", ((struct bow_config *)bow_config)->window_timeout);
    g_timeout_add(((struct bow_config *)bow_config)->window_timeout, close_window_callback, gtk_window);
    gtk_window_present(gtk_window);

    g_signal_connect(gtk_window, "destroy", G_CALLBACK(gtk_window_close), NULL);
}

void bow_create_run_window(gpointer bow_config) {
    // bow_log_debug("Received string: %s", expression);
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
