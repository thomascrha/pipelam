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

/* Global variable to track current active window */
static GtkWidget *current_window = NULL;

/* Function to check if there's an active window */
int pipelam_has_active_window(void) { return (current_window != NULL && GTK_IS_WINDOW(current_window)); }

/* Function to close the current window if one exists */
void pipelam_close_current_window(void) {
    if (current_window != NULL) {
        pipelam_log_debug("Closing current window due to new message");
        if (GTK_IS_WINDOW(current_window)) {
            gtk_window_close(GTK_WINDOW(current_window));
        } else {
            pipelam_log_error("Invalid window pointer in pipelam_close_current_window");
        }
        current_window = NULL;
    }
}

static gboolean *pipelam_get_anchor(enum pipelam_window_anchor anchor) {
    // Allocate memory for GTK_LAYER_SHELL_EDGE_LEFT, RIGHT, TOP, BOTTOM
    gboolean *anchors = (gboolean *)malloc(4 * sizeof(gboolean));

    if (anchors == NULL) {
        pipelam_log_error("Memory allocation failed");
        return NULL;
    }

    // Initialize all anchors to FALSE
    // [0]=LEFT, [1]=RIGHT, [2]=TOP, [3]=BOTTOM
    for (int i = 0; i < 4; i++) {
        anchors[i] = FALSE;
    }

    pipelam_log_debug("current anchor: %d", anchor);

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
        pipelam_log_error("Unknown anchor: %d, using center", anchor);
        // All anchors remain FALSE
        break;
    }

    return anchors;
}

static GtkWindow *pipelam_render_gtk_window(GtkApplication *app, gpointer pipelam_config) {
    pipelam_log_debug("Creating window");
    pipelam_log_debug("Received string: %s", ((struct pipelam_config *)pipelam_config)->expression);

    if (((struct pipelam_config *)pipelam_config)->expression == NULL) {
        pipelam_log_error("data is NULL");
        return NULL;
    }

    GtkWidget *window = gtk_application_window_new(app);
    if (window == NULL) {
        pipelam_log_error("gtk_application_window_new() returned NULL");
        return NULL;
    }

    GtkWindow *gtk_window = GTK_WINDOW(window);

    // Before the window is first realized, set it up to be a layer surface
    gtk_layer_init_for_window(gtk_window);

    // Order below normal windows
    gtk_layer_set_layer(gtk_window, GTK_LAYER_SHELL_LAYER_TOP);

    // Push other windows out of the way
    gtk_layer_auto_exclusive_zone_enable(gtk_window);

    // Set margins
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_LEFT, ((struct pipelam_config *)pipelam_config)->margin_left);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_RIGHT, ((struct pipelam_config *)pipelam_config)->margin_right);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_TOP, ((struct pipelam_config *)pipelam_config)->margin_top);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_BOTTOM, ((struct pipelam_config *)pipelam_config)->margin_bottom);

    // Set anchors
    gboolean *anchors = pipelam_get_anchor(((struct pipelam_config *)pipelam_config)->anchor);
    if (anchors != NULL) {
        // Explicitly use the GTK_LAYER_SHELL_EDGE_* constants for clarity
        gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_LEFT, anchors[GTK_LAYER_SHELL_EDGE_LEFT]);
        gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_RIGHT, anchors[GTK_LAYER_SHELL_EDGE_RIGHT]);
        gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_TOP, anchors[GTK_LAYER_SHELL_EDGE_TOP]);
        gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_BOTTOM, anchors[GTK_LAYER_SHELL_EDGE_BOTTOM]);

        // Debug logging
        for (int i = 0; i < 4; i++) {
            pipelam_log_debug("anchors[%d]: %d", i, anchors[i]);
        }

        free(anchors); // Free memory after use
    }

    return gtk_window;
}

static gboolean close_window_callback(gpointer window) {
    if (GTK_IS_WINDOW(window)) {
        pipelam_log_debug("Closing window via timeout");
        gtk_window_close(GTK_WINDOW(window));
        // Clear the current_window pointer if this is the current window
        if (current_window == window) {
            current_window = NULL;
        }
    } else {
        pipelam_log_error("Invalid window pointer in close_window_callback");
    }
    return G_SOURCE_REMOVE; // Return FALSE to remove the source
}

// Custom handler for window destroy signals
static void on_window_destroy(GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
    pipelam_log_debug("Window destroyed");
    if (current_window == widget) {
        current_window = NULL;
    }
}

static void pipelam_render_image_window(GtkApplication *app, gpointer user_data) {
    pipelam_log_debug("Creating image window");

    // Get the config from user_data
    struct pipelam_config *pipelam_config = (struct pipelam_config *)user_data;

    GtkWindow *gtk_window = pipelam_render_gtk_window(app, pipelam_config);
    if (gtk_window == NULL) {
        pipelam_log_error("gtk_render_gtk_window() returned NULL");
        return;
    }

    // Store the new window as the current window
    current_window = GTK_WIDGET(gtk_window);

    const char *image_path = ((struct pipelam_config *)pipelam_config)->expression;
    pipelam_log_debug("Loading image from path: %s", image_path);

    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path, &error);
    if (pixbuf == NULL) {
        pipelam_log_error("Failed to load image: %s", error ? error->message : "Unknown error");
        if (error)
            g_error_free(error);
        return;
    }

    gint width = gdk_pixbuf_get_width(pixbuf);
    gint height = gdk_pixbuf_get_height(pixbuf);
    pipelam_log_debug("Image dimensions - width: %d, height: %d", width, height);

    // Create paintable from pixbuf
    GdkPaintable *paintable = GDK_PAINTABLE(gdk_texture_new_for_pixbuf(pixbuf));
    if (paintable == NULL) {
        pipelam_log_error("Failed to create paintable from pixbuf");
        g_object_unref(pixbuf);
        return;
    }

    // Create image widget from paintable
    GtkWidget *image = gtk_image_new_from_paintable(paintable);
    if (image == NULL) {
        pipelam_log_error("Failed to create image widget");
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

    pipelam_log_debug("window_timeout: %d", ((struct pipelam_config *)pipelam_config)->window_timeout);
    g_timeout_add(((struct pipelam_config *)pipelam_config)->window_timeout, close_window_callback, gtk_window);
    gtk_window_present(gtk_window);

    g_signal_connect(gtk_window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    // Unref paintable and pixbuf after they're used
    g_object_unref(paintable);
    g_object_unref(pixbuf);
}

static void pipelam_render_text_window(GtkApplication *app, gpointer user_data) {
    pipelam_log_debug("Creating text window");

    // Get the config from user_data
    struct pipelam_config *pipelam_config = (struct pipelam_config *)user_data;

    GtkWindow *gtk_window = pipelam_render_gtk_window(app, pipelam_config);
    if (gtk_window == NULL) {
        pipelam_log_error("gtk_render_gtk_window() returned NULL");
        return;
    }

    // Store the new window as the current window
    current_window = GTK_WIDGET(gtk_window);

    GtkWidget *label = gtk_label_new(NULL);
    if (label == NULL) {
        pipelam_log_error("gtk_label_new() returned NULL");
        return;
    }

    // print len of expression
    pipelam_log_debug("len of expression: %lu", strlen(((struct pipelam_config *)pipelam_config)->expression));
    pipelam_log_debug("expression: %s", ((struct pipelam_config *)pipelam_config)->expression);

    // combine markup and border
    gtk_label_set_markup(GTK_LABEL(label), ((struct pipelam_config *)pipelam_config)->expression);
    gtk_window_set_child(gtk_window, label);

    pipelam_log_debug("window_timeout: %d", ((struct pipelam_config *)pipelam_config)->window_timeout);
    g_timeout_add(((struct pipelam_config *)pipelam_config)->window_timeout, close_window_callback, gtk_window);
    gtk_window_present(gtk_window);

    g_signal_connect(gtk_window, "destroy", G_CALLBACK(on_window_destroy), NULL);
}

void pipelam_create_run_window(gpointer pipelam_config) {
    pipelam_log_debug("Creating run window");
    GtkApplication *app = gtk_application_new("com.github.wmww.pipelam", G_APPLICATION_DEFAULT_FLAGS);

    if (((struct pipelam_config *)pipelam_config)->type == IMAGE) {
        g_signal_connect(app, "activate", G_CALLBACK(pipelam_render_image_window), pipelam_config);
    } else if (((struct pipelam_config *)pipelam_config)->type == TEXT) {
        g_signal_connect(app, "activate", G_CALLBACK(pipelam_render_text_window), pipelam_config);
    } else {
        pipelam_log_error("Unknown type: %d", ((struct pipelam_config *)pipelam_config)->type);
        return;
    }
    g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);
}
