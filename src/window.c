#include "gtk4-layer-shell.h"

#include "config.h"
#include "log.h"

static GtkWindow *current_window = NULL;
static GtkApplication *app = NULL;

static gboolean *pipelam_get_anchor(enum pipelam_window_anchor anchor) {
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

void pipelam_close_current_window(void) {
    if (current_window != NULL) {
        pipelam_log_debug("Closing current window");
        gtk_window_close(current_window);
        current_window = NULL;
    } else {
        pipelam_log_debug("No current window to close");
    }
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

    gtk_layer_init_for_window(gtk_window);
    gtk_layer_set_layer(gtk_window, GTK_LAYER_SHELL_LAYER_TOP);
    gtk_layer_auto_exclusive_zone_enable(gtk_window);

    // Set margins
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_LEFT, ((struct pipelam_config *)pipelam_config)->margin_left);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_RIGHT, ((struct pipelam_config *)pipelam_config)->margin_right);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_TOP, ((struct pipelam_config *)pipelam_config)->margin_top);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_BOTTOM, ((struct pipelam_config *)pipelam_config)->margin_bottom);

    gboolean *anchors = pipelam_get_anchor(((struct pipelam_config *)pipelam_config)->anchor);
    if (anchors != NULL) {
        // Explicitly use the GTK_LAYER_SHELL_EDGE_* constants for clarity
        gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_LEFT, anchors[GTK_LAYER_SHELL_EDGE_LEFT]);
        gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_RIGHT, anchors[GTK_LAYER_SHELL_EDGE_RIGHT]);
        gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_TOP, anchors[GTK_LAYER_SHELL_EDGE_TOP]);
        gtk_layer_set_anchor(gtk_window, GTK_LAYER_SHELL_EDGE_BOTTOM, anchors[GTK_LAYER_SHELL_EDGE_BOTTOM]);

        for (int i = 0; i < 4; i++) {
            pipelam_log_debug("anchors[%d]: %d", i, anchors[i]);
        }

        free(anchors);
    }

    return gtk_window;
}

static gboolean close_window_callback(gpointer window) {
    if (GTK_IS_WINDOW(window)) {
        pipelam_log_debug("Closing window via timeout");
        gtk_window_close(GTK_WINDOW(window));
    }
    return G_SOURCE_REMOVE; // Return FALSE to remove the source
}

static void pipelam_render_wob_window(GtkApplication *app, gpointer user_data) {
    pipelam_log_debug("Creating wob window");
    struct pipelam_config *pipelam_config = (struct pipelam_config *)user_data;

    GtkWindow *gtk_window = pipelam_render_gtk_window(app, pipelam_config);
    if (gtk_window == NULL) {
        pipelam_log_error("gtk_render_gtk_window() returned NULL");
        return;
    }

    // Parse the percentage value (0-100)
    int percentage = 0;
    if (pipelam_config->expression != NULL) {
        percentage = atoi(pipelam_config->expression);
        // Clamp to valid range
        if (percentage < 0)
            percentage = 0;
        if (percentage > 100)
            percentage = 100;
    }
    pipelam_log_debug("WOB value: %d%%", percentage);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *bar_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    // Create the background of the bar with increased size
    GtkWidget *bar_bg = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(bar_bg, TRUE);
    gtk_widget_set_size_request(bar_bg, 350, 35);

    // Create the foreground (the actual volume indicator)
    GtkWidget *bar_fg = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    int bar_width = (350 * percentage) / 100;
    gtk_widget_set_size_request(bar_fg, bar_width, 35);

    // Create an outer container with padding for the border effect
    GtkWidget *border_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    gtk_widget_add_css_class(border_container, "wob-border");
    gtk_widget_add_css_class(bar_bg, "wob-background");
    gtk_widget_add_css_class(bar_fg, "wob-foreground");
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css_data = ".wob-border { background-color: white; padding: 10px; margin: 3px; }"
                           ".wob-background { background-color: black; }"
                           ".wob-foreground { background-color: white; }";

    gtk_css_provider_load_from_string(provider, css_data);

    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_box_append(GTK_BOX(bar_bg), bar_fg);
    gtk_box_append(GTK_BOX(border_container), bar_bg);
    gtk_box_append(GTK_BOX(bar_container), border_container);
    gtk_box_append(GTK_BOX(box), bar_container);

    gtk_window_set_child(gtk_window, box);

    pipelam_log_debug("window_timeout: %d", pipelam_config->window_timeout);
    g_timeout_add(pipelam_config->window_timeout, close_window_callback, gtk_window);
    gtk_window_present(gtk_window);

    current_window = gtk_window;
    g_object_unref(provider);
}

static void pipelam_render_image_window(GtkApplication *app, gpointer user_data) {
    pipelam_log_debug("Creating image window");
    struct pipelam_config *pipelam_config = (struct pipelam_config *)user_data;

    GtkWindow *gtk_window = pipelam_render_gtk_window(app, pipelam_config);
    if (gtk_window == NULL) {
        pipelam_log_error("gtk_render_gtk_window() returned NULL");
        return;
    }

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
    gtk_window_set_default_size(gtk_window, width, height);

    pipelam_log_debug("window_timeout: %d", ((struct pipelam_config *)pipelam_config)->window_timeout);
    g_timeout_add(((struct pipelam_config *)pipelam_config)->window_timeout, close_window_callback, gtk_window);
    gtk_window_present(gtk_window);

    current_window = gtk_window;

    g_object_unref(paintable);
    g_object_unref(pixbuf);
}

static void pipelam_render_text_window(GtkApplication *app, gpointer user_data) {
    pipelam_log_debug("Creating text window");
    struct pipelam_config *pipelam_config = (struct pipelam_config *)user_data;

    GtkWindow *gtk_window = pipelam_render_gtk_window(app, pipelam_config);
    if (gtk_window == NULL) {
        pipelam_log_error("gtk_render_gtk_window() returned NULL");
        return;
    }

    GtkWidget *label = gtk_label_new(NULL);
    if (label == NULL) {
        pipelam_log_error("gtk_label_new() returned NULL");
        return;
    }

    pipelam_log_debug("len of expression: %lu", strlen(((struct pipelam_config *)pipelam_config)->expression));
    pipelam_log_debug("expression: %s", ((struct pipelam_config *)pipelam_config)->expression);

    gtk_label_set_markup(GTK_LABEL(label), ((struct pipelam_config *)pipelam_config)->expression);
    gtk_window_set_child(gtk_window, label);

    pipelam_log_debug("window_timeout: %d", ((struct pipelam_config *)pipelam_config)->window_timeout);
    g_timeout_add(((struct pipelam_config *)pipelam_config)->window_timeout, close_window_callback, gtk_window);
    gtk_window_present(gtk_window);

    current_window = gtk_window;
}

void pipelam_create_window(gpointer pipelam_config) {
    pipelam_log_debug("Creating window");
    app = gtk_application_new("com.github.thomascrha.pipelam", G_APPLICATION_NON_UNIQUE);
    g_application_register(G_APPLICATION(app), NULL, NULL);

    if (((struct pipelam_config *)pipelam_config)->type == IMAGE) {
        if (((struct pipelam_config *)pipelam_config)->runtime_behaviour == QUEUE) {
            g_signal_connect(app, "activate", G_CALLBACK(pipelam_render_image_window), pipelam_config);
            g_application_run(G_APPLICATION(app), 0, NULL);
        } else {
            pipelam_render_image_window(app, pipelam_config);
        }
    } else if (((struct pipelam_config *)pipelam_config)->type == TEXT) {
        if (((struct pipelam_config *)pipelam_config)->runtime_behaviour == QUEUE) {
            g_signal_connect(app, "activate", G_CALLBACK(pipelam_render_text_window), pipelam_config);
            g_application_run(G_APPLICATION(app), 0, NULL);
        } else {
            pipelam_render_text_window(app, pipelam_config);
        }
    } else if (((struct pipelam_config *)pipelam_config)->type == WOB) {
        if (((struct pipelam_config *)pipelam_config)->runtime_behaviour == QUEUE) {
            g_signal_connect(app, "activate", G_CALLBACK(pipelam_render_wob_window), pipelam_config);
            g_application_run(G_APPLICATION(app), 0, NULL);
        } else {
            pipelam_render_wob_window(app, pipelam_config);
        }
    } else {
        pipelam_log_error("Unknown type: %d", ((struct pipelam_config *)pipelam_config)->type);
    }

    g_object_unref(app);
}
