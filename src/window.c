#include "gtk4-layer-shell.h"

#include "config.h"
#include "log.h"

static GtkWindow *current_window = NULL; // Track the current window
static GtkApplication *app = NULL;

// Internal functions
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

static void pipelam_close_window(GtkWindow *window) {
    if (!GTK_IS_WINDOW(window))
        return;

    // Get and remove the timeout associated with this window
    guint timeout_id = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(window), "pipelam-timeout-id"));
    if (timeout_id > 0) {
        pipelam_log_debug("Removing timeout (ID: %u) for window", timeout_id);
        g_source_remove(timeout_id);
        g_object_set_data(G_OBJECT(window), "pipelam-timeout-id", GUINT_TO_POINTER(0));
    }

    pipelam_log_debug("Closing window");
    gtk_window_close(window);

    if (window == current_window) {
        current_window = NULL;

    }
}

static void pipelam_update_window_settings(GtkWindow *window, struct pipelam_config *config) {
    if (!GTK_IS_WINDOW(window) || config == NULL)
        return;

    gtk_layer_set_margin(window, GTK_LAYER_SHELL_EDGE_LEFT, config->margin_left);
    gtk_layer_set_margin(window, GTK_LAYER_SHELL_EDGE_RIGHT, config->margin_right);
    gtk_layer_set_margin(window, GTK_LAYER_SHELL_EDGE_TOP, config->margin_top);
    gtk_layer_set_margin(window, GTK_LAYER_SHELL_EDGE_BOTTOM, config->margin_bottom);

    gboolean *anchors = pipelam_get_anchor(config->anchor);
    if (anchors != NULL) {
        gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_LEFT, anchors[GTK_LAYER_SHELL_EDGE_LEFT]);
        gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_RIGHT, anchors[GTK_LAYER_SHELL_EDGE_RIGHT]);
        gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_TOP, anchors[GTK_LAYER_SHELL_EDGE_TOP]);
        gtk_layer_set_anchor(window, GTK_LAYER_SHELL_EDGE_BOTTOM, anchors[GTK_LAYER_SHELL_EDGE_BOTTOM]);

        free(anchors);
    }
}

static gboolean close_window_callback(gpointer window) {
    if (GTK_IS_WINDOW(window)) {
        pipelam_log_debug("Closing window via timeout");
        pipelam_close_window(GTK_WINDOW(window));
    }
    return G_SOURCE_REMOVE;
}
static void pipelam_reset_window_timeout(GtkWindow *window, guint window_timeout) {
    if (!GTK_IS_WINDOW(window))
        return;

    // Get existing timeout
    guint old_timeout_id = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(window), "pipelam-timeout-id"));

    if (old_timeout_id > 0) {
        pipelam_log_debug("Removing timeout (ID: %u)", old_timeout_id);
        g_source_remove(old_timeout_id);
    }

    guint new_timeout_id = g_timeout_add(window_timeout, close_window_callback, window);
    pipelam_log_debug("Set new timeout (ID: %u)", new_timeout_id);

    // Store the new timeout in the window's data
    g_object_set_data(G_OBJECT(window), "pipelam-timeout-id", GUINT_TO_POINTER(new_timeout_id));
}

static GtkWindow *pipelam_render_gtk_window(GtkApplication *app, gpointer ptr_pipelam_config) {
    struct pipelam_config *pipelam_config = (struct pipelam_config *)ptr_pipelam_config;

    pipelam_log_debug("Received string: %s", pipelam_config->expression);

    if (pipelam_config->expression == NULL) {
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

    pipelam_update_window_settings(gtk_window, pipelam_config);

    return gtk_window;
}

static GtkWidget *pipelam_load_and_create_image(const char *image_path, gint *width, gint *height) {
    pipelam_log_debug("Loading image from path: %s", image_path);

    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(image_path, &error);
    if (pixbuf == NULL) {
        pipelam_log_error("Failed to load image: %s", error ? error->message : "Unknown error");
        if (error)
            g_error_free(error);
        return NULL;
    }

    *width = gdk_pixbuf_get_width(pixbuf);
    *height = gdk_pixbuf_get_height(pixbuf);

    GdkPaintable *paintable = GDK_PAINTABLE(gdk_texture_new_for_pixbuf(pixbuf));
    GtkWidget *image = gtk_image_new_from_paintable(paintable);

    gtk_widget_set_size_request(image, *width, *height);
    gtk_image_set_pixel_size(GTK_IMAGE(image), -1); // Use natural size

    g_object_unref(paintable);
    g_object_unref(pixbuf);

    return image;
}

static void pipelam_update_image_window(GtkWindow *window, const char *image_path, struct pipelam_config *config) {
    if (!GTK_IS_WINDOW(window) || image_path == NULL)
        return;

    pipelam_log_debug("Updating existing IMAGE window");

    GtkWidget *box = gtk_window_get_child(window);
    if (GTK_IS_BOX(box)) {
        GtkWidget *old_image = gtk_widget_get_first_child(box);
        if (old_image != NULL) {
            gtk_box_remove(GTK_BOX(box), old_image);
        }

        gint width = 0, height = 0;
        GtkWidget *image = pipelam_load_and_create_image(image_path, &width, &height);
        if (image != NULL) {
            gtk_box_append(GTK_BOX(box), image);
            gtk_window_set_default_size(window, width, height);
        }
    }

    pipelam_update_window_settings(window, config);

    pipelam_reset_window_timeout(window, config->window_timeout);
    gtk_window_present(window);
}

static void pipelam_update_text_window(GtkWindow *window, const char *text, struct pipelam_config *config) {
    if (!GTK_IS_WINDOW(window) || text == NULL)
        return;

    pipelam_log_debug("Updating existing TEXT window");
    GtkWidget *old_label = gtk_window_get_child(window);
    if (GTK_IS_LABEL(old_label)) {
        pipelam_log_debug("len of expression: %lu", strlen(text));
        pipelam_log_debug("expression: %s", text);

        gtk_label_set_markup(GTK_LABEL(old_label), text);
    }

    pipelam_update_window_settings(window, config);

    gtk_window_present(window);
}

static int pipelam_get_bar_width(int percentage, struct pipelam_config *config) {
    int bar_width;
    int wob_bar_width = config->wob_bar_width;
    // Calculate effective margin from padding values
    int box_padding = config->wob_box_padding;
    int border_padding = config->wob_border_padding;
    int effective_margin = box_padding + border_padding;

    // Handle full bar for 200%
    if (percentage >= 200) {
        return wob_bar_width - (2 * effective_margin); // Full bar width - minus margins
    }

    // For values over 100%, wrap around (e.g., 150% displays as 50%)
    int display_percentage = percentage > 100 ? percentage % 100 : percentage;

    if (display_percentage == 100 || display_percentage == 0) {
        // Edge case: 100% or 0% (from 200%)
        bar_width = display_percentage == 100 ? wob_bar_width - (2 * effective_margin) : 0;
    } else {
        bar_width = (wob_bar_width * display_percentage) / 100;
    }
    return bar_width;
}

// Main render functions for each type
static void pipelam_render_wob_window(GtkApplication *app, gpointer ptr_pipelam_config) {
    struct pipelam_config *pipelam_config = (struct pipelam_config *)ptr_pipelam_config;

    // debug print all the wob settings
    pipelam_log_debug("WOB settings:");
    pipelam_log_debug("wob_bar_width: %d", pipelam_config->wob_bar_width);
    pipelam_log_debug("wob_bar_height: %d", pipelam_config->wob_bar_height);
    pipelam_log_debug("wob_border_color: %s", pipelam_config->wob_border_color);
    pipelam_log_debug("wob_background_color: %s", pipelam_config->wob_background_color);
    pipelam_log_debug("wob_foreground_color: %s", pipelam_config->wob_foreground_color);
    pipelam_log_debug("wob_box_color: %s", pipelam_config->wob_box_color);
    pipelam_log_debug("wob_border_padding: %d", pipelam_config->wob_border_padding);
    pipelam_log_debug("wob_border_margin: %d", pipelam_config->wob_border_margin);
    pipelam_log_debug("wob_background_padding: %d", pipelam_config->wob_background_padding);
    pipelam_log_debug("wob_foreground_padding: %d", pipelam_config->wob_foreground_padding);
    pipelam_log_debug("wob_foreground_overflow_padding: %d", pipelam_config->wob_foreground_overflow_padding);

    int percentage = 0;
    if (pipelam_config->expression != NULL) {
        percentage = atoi(pipelam_config->expression);
        if (percentage < 0)
            percentage = 0;
        if (percentage > 200)
            percentage = 200;
    }
    pipelam_log_debug("WOB value: %d%%", percentage);

    if (pipelam_config->runtime_behaviour != OVERLAY && current_window != NULL) {
        GtkWidget *bar_fg = GTK_WIDGET(g_object_get_data(G_OBJECT(current_window), "pipelam-bar-fg"));
        if (GTK_IS_WIDGET(bar_fg)) {
            pipelam_log_debug("Overlaying existing WOB window");

            int bar_width = pipelam_get_bar_width(percentage, pipelam_config);
            gtk_widget_set_size_request(bar_fg, bar_width, pipelam_config->wob_bar_height);

            // Change CSS class based on percentage value
            if (percentage > 100) {
                gtk_widget_remove_css_class(bar_fg, "wob-foreground");
                gtk_widget_add_css_class(bar_fg, "wob-foreground-overflow");

                // If it's 200%, make it a full red bar
                if (percentage >= 200) {
                    // Calculate effective margin from padding values
                    int box_padding = pipelam_config->wob_box_padding;
                    int border_padding = pipelam_config->wob_border_padding;
                    int effective_margin = box_padding + border_padding;
                    gtk_widget_set_size_request(bar_fg, pipelam_config->wob_bar_width - effective_margin, pipelam_config->wob_bar_height); // Full width
                }
            } else {
                gtk_widget_remove_css_class(bar_fg, "wob-foreground-overflow");
                gtk_widget_add_css_class(bar_fg, "wob-foreground");
            }

            pipelam_update_window_settings(current_window, pipelam_config);

            // Reset the window timeout
            pipelam_reset_window_timeout(current_window, pipelam_config->window_timeout);
            pipelam_log_debug("Reset WOB window timeout");

            gtk_window_present(current_window);
            return;
        }
    } else if (current_window != NULL) {
        GtkWidget *bar_fg = GTK_WIDGET(g_object_get_data(G_OBJECT(current_window), "pipelam-bar-fg"));
        if (GTK_IS_WIDGET(bar_fg)) {
            pipelam_log_debug("Updating existing WOB window");

            int bar_width = pipelam_get_bar_width(percentage, pipelam_config);
            gtk_widget_set_size_request(bar_fg, bar_width, pipelam_config->wob_bar_height);

            pipelam_update_window_settings(current_window, pipelam_config);

            // Reset the window timeout
            pipelam_reset_window_timeout(current_window, pipelam_config->window_timeout);
            pipelam_log_debug("Reset timeout");

            gtk_window_present(current_window);
            return;
        }
    }

    pipelam_log_debug("Creating new WOB window");
    GtkWindow *gtk_window = pipelam_render_gtk_window(app, pipelam_config);
    if (gtk_window == NULL) {
        pipelam_log_error("gtk_render_gtk_window() returned NULL");
        return;
    }

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *bar_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_add_css_class(box, "wob-box");

    GtkWidget *bar_bg = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(bar_bg, TRUE);
    gtk_widget_set_size_request(bar_bg, pipelam_config->wob_bar_width, pipelam_config->wob_bar_height);

    GtkWidget *new_bar_fg = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    int bar_width = pipelam_get_bar_width(percentage, pipelam_config);
    gtk_widget_set_size_request(new_bar_fg, bar_width, pipelam_config->wob_bar_height);

    if (percentage > 100) {
        gtk_widget_add_css_class(new_bar_fg, "wob-foreground-overflow");
        // If it's 200%, make it a full red bar
        if (percentage >= 200) {
            // Calculate effective margin from padding values
            int box_padding = pipelam_config->wob_box_padding;
            int border_padding = pipelam_config->wob_border_padding;
            int effective_margin = box_padding + border_padding;
            gtk_widget_set_size_request(new_bar_fg, pipelam_config->wob_bar_width - effective_margin, pipelam_config->wob_bar_height); // Full width
        }
    } else {
        gtk_widget_add_css_class(new_bar_fg, "wob-foreground");
    }

    GtkWidget *border_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    gtk_widget_add_css_class(border_container, "wob-border");
    gtk_widget_add_css_class(bar_bg, "wob-background");

    GtkCssProvider *provider = gtk_css_provider_new();
    char css_data[512];
    snprintf(css_data, sizeof(css_data),
             ".wob-box { background-color: %s; padding: %dpx; }"
             ".wob-border { background-color: %s; padding: %dpx; margin: %dpx; }"
             ".wob-background { background-color: %s; padding: %dpx; }"
             ".wob-foreground { background-color: %s; padding: %dpx; }"
             ".wob-foreground-overflow { background-color: %s; padding: %dpx; }",
             pipelam_config->wob_box_color, pipelam_config->wob_box_padding, pipelam_config->wob_border_color, pipelam_config->wob_border_padding, pipelam_config->wob_border_margin,
             pipelam_config->wob_background_color, pipelam_config->wob_background_padding, pipelam_config->wob_foreground_color, pipelam_config->wob_foreground_padding,
             pipelam_config->wob_overflow_color, pipelam_config->wob_foreground_overflow_padding);

    gtk_css_provider_load_from_string(provider, css_data);

    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_box_append(GTK_BOX(bar_bg), new_bar_fg);
    gtk_box_append(GTK_BOX(border_container), bar_bg);
    gtk_box_append(GTK_BOX(bar_container), border_container);
    gtk_box_append(GTK_BOX(box), bar_container);

    gtk_window_set_child(gtk_window, box);

    pipelam_log_debug("window_timeout: %d", pipelam_config->window_timeout);

    // If there's already a window, close it
    if (current_window != NULL) {
        pipelam_log_debug("Closing previous window");
        pipelam_close_window(current_window);
    }

    current_window = gtk_window;
    g_object_set_data(G_OBJECT(gtk_window), "pipelam-bar-fg", new_bar_fg);
    pipelam_reset_window_timeout(gtk_window, pipelam_config->window_timeout);
    pipelam_log_debug("Set new WOB window timeout");

    gtk_window_present(gtk_window);
    g_object_unref(provider);
}

static void pipelam_render_image_window(GtkApplication *app, gpointer ptr_pipelam_config) {
    pipelam_log_debug("Handling image window");
    struct pipelam_config *pipelam_config = (struct pipelam_config *)ptr_pipelam_config;

    const char *image_path = pipelam_config->expression;

    if (pipelam_config->runtime_behaviour != OVERLAY && current_window != NULL) {
        // In non-overlay mode, we can update the existing window regardless of type
        // We'll treat it as an image window now
        pipelam_update_image_window(current_window, image_path, pipelam_config);
        return;
    }

    pipelam_log_debug("Creating new image window");
    GtkWindow *gtk_window = pipelam_render_gtk_window(app, pipelam_config);
    if (gtk_window == NULL) {
        pipelam_log_error("gtk_render_gtk_window() returned NULL");
        return;
    }

    gint width = 0, height = 0;
    GtkWidget *image = pipelam_load_and_create_image(image_path, &width, &height);

    if (image == NULL) {
        pipelam_log_error("Failed to load image");
        return;
    }

    pipelam_log_debug("Image dimensions - width: %d, height: %d", width, height);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_append(GTK_BOX(box), image);
    gtk_window_set_child(gtk_window, box);
    gtk_window_set_default_size(gtk_window, width, height);

    // If there's already a window, close it
    if (pipelam_config->runtime_behaviour == REPLACE && current_window != NULL) {
        pipelam_log_debug("Closing previous window");
        pipelam_close_window(current_window);
    }

    current_window = gtk_window;
    pipelam_reset_window_timeout(gtk_window, pipelam_config->window_timeout);

    gtk_window_present(gtk_window);
}

static void pipelam_render_text_window(GtkApplication *app, gpointer ptr_pipelam_config) {
    pipelam_log_debug("Handling text window");
    struct pipelam_config *pipelam_config = (struct pipelam_config *)ptr_pipelam_config;

    if (pipelam_config->runtime_behaviour != OVERLAY && current_window != NULL) {
        // In non-overlay mode, we can update the existing window regardless of type
        // We'll treat it as a text window now
        pipelam_update_text_window(current_window, pipelam_config->expression, pipelam_config);
        return;
    }

    pipelam_log_debug("Creating new text window");
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

    pipelam_log_debug("len of expression: %lu", strlen(pipelam_config->expression));
    pipelam_log_debug("expression: %s", pipelam_config->expression);

    gtk_label_set_markup(GTK_LABEL(label), pipelam_config->expression);
    gtk_window_set_child(gtk_window, label);

    // If we're replacing a window, close the old one
    if (pipelam_config->runtime_behaviour == REPLACE && current_window != NULL) {
        pipelam_log_debug("Closing previous window");
        pipelam_close_window(current_window);
    }

    current_window = gtk_window;
    pipelam_reset_window_timeout(gtk_window, pipelam_config->window_timeout);

    gtk_window_present(gtk_window);
}

// External functions
void pipelam_set_application(GtkApplication *application) { app = application; }

gboolean pipelam_create_window(gpointer ptr_pipelam_config) {
    struct pipelam_config *pipelam_config = (struct pipelam_config *)ptr_pipelam_config;
    pipelam_log_debug("Creating window");

    // Make a persistent copy of the config to avoid the expression being nulled
    static struct pipelam_config saved_config;
    memcpy(&saved_config, pipelam_config, sizeof(struct pipelam_config));
    if (pipelam_config->expression != NULL) {
        saved_config.expression = g_strdup(pipelam_config->expression);
    }
    ptr_pipelam_config = &saved_config;
    pipelam_config = &saved_config;

    if (app == NULL) {
        pipelam_log_error("No GTK application available");

        return FALSE;
    }



    if (pipelam_config->runtime_behaviour == QUEUE && current_window != NULL) {
        g_timeout_add(10, pipelam_create_window, ptr_pipelam_config);
        return FALSE;
    }

    if (pipelam_config->type == IMAGE) {
        pipelam_render_image_window(app, ptr_pipelam_config);
    } else if (pipelam_config->type == TEXT) {
        pipelam_render_text_window(app, ptr_pipelam_config);
    } else if (pipelam_config->type == WOB) {
        pipelam_render_wob_window(app, ptr_pipelam_config);
    } else {
        pipelam_log_error("Unknown type: %d", pipelam_config->type);
    }


    // Clean up the saved expression after window creation is complete
    // This happens after the window is created, not when this function returns
    if (saved_config.expression != NULL && saved_config.expression != pipelam_config->expression) {
        g_free(saved_config.expression);
        saved_config.expression = NULL;
    }

    return G_SOURCE_REMOVE;
}
