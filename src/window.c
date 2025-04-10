#include "gtk4-layer-shell.h"

#include "config.h"
#include "log.h"

typedef struct {
    GtkWindow *window;
    enum pipelam_message_type window_type;
    guint timeout_id;
    GtkWidget *bar_fg; // For WOB windows
} PipelamWindow;

#define MAX_OVERLAY_WINDOWS 10
static PipelamWindow window_list[MAX_OVERLAY_WINDOWS] = {0};
static int window_count = 0;

static GtkWindow *current_window = NULL; // Still track current for non-overlay mode
static GtkApplication *app = NULL;
static guint current_timeout_id = 0;
static enum pipelam_message_type current_window_type = -1; // Track the current window type
static GtkWidget *bar_fg = NULL;                           // Keep reference to the WOB foreground bar for non-overlay mode
static gboolean is_updating_window = FALSE;                // Flag to prevent concurrent window updates

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

static void pipelam_close_window_by_index(int index) {
    if (index < 0 || index >= window_count)
        return;

    PipelamWindow *win = &window_list[index];

    if (win->timeout_id > 0) {
        pipelam_log_debug("Removing timeout (ID: %u) for window %d", win->timeout_id, index);
        g_source_remove(win->timeout_id);
        win->timeout_id = 0;
    }

    if (win->window != NULL) {
        pipelam_log_debug("Closing window %d of type %d", index, win->window_type);
        gtk_window_close(win->window);
    }

    // Shift remaining windows to fill the gap
    if (index < window_count - 1) {
        memmove(&window_list[index], &window_list[index + 1], (window_count - index - 1) * sizeof(PipelamWindow));
    }

    // Clear the last entry
    memset(&window_list[window_count - 1], 0, sizeof(PipelamWindow));
    window_count--;
}

static void pipelam_add_window_to_list(GtkWindow *window, enum pipelam_message_type type, guint timeout_id, GtkWidget *wob_fg) {
    if (window_count >= MAX_OVERLAY_WINDOWS) {
        pipelam_log_warning("Maximum number of overlay windows reached, closing oldest");
        pipelam_close_window_by_index(0);
    }

    window_list[window_count].window = window;
    window_list[window_count].window_type = type;
    window_list[window_count].timeout_id = timeout_id;
    window_list[window_count].bar_fg = wob_fg;
    window_count++;

    pipelam_log_debug("Added window to list at index %d, total windows: %d", window_count - 1, window_count);
}

static int pipelam_find_window_index(GtkWindow *window) {
    for (int i = 0; i < window_count; i++) {
        if (window_list[i].window == window)
            return i;
    }
    return -1;
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
        int index = pipelam_find_window_index(GTK_WINDOW(window));
        if (index >= 0) {
            pipelam_log_debug("Closing window %d from overlay list", index);
            pipelam_close_window_by_index(index);
        } else if (window == current_window) {
            bar_fg = NULL;
            current_window = NULL;
            current_window_type = -1;
            current_timeout_id = 0;
            gtk_window_close(GTK_WINDOW(window));
        } else {
            gtk_window_close(GTK_WINDOW(window));
        }
    }
    return G_SOURCE_REMOVE;
}
static guint pipelam_reset_window_timeout(GtkWindow *window, guint old_timeout_id, guint window_timeout) {
    if (!GTK_IS_WINDOW(window))
        return 0;

    if (old_timeout_id > 0) {
        pipelam_log_debug("Removing timeout (ID: %u)", old_timeout_id);
        g_source_remove(old_timeout_id);
    }

    guint new_timeout_id = g_timeout_add(window_timeout, close_window_callback, window);
    pipelam_log_debug("Set new timeout (ID: %u)", new_timeout_id);

    return new_timeout_id;
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

    if (window == current_window) {
        current_timeout_id = pipelam_reset_window_timeout(window, current_timeout_id, config->window_timeout);
    } else {
        int index = pipelam_find_window_index(window);
        if (index >= 0) {
            window_list[index].timeout_id = pipelam_reset_window_timeout(window, window_list[index].timeout_id, config->window_timeout);
        }
    }

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

    if (window == current_window) {
        current_timeout_id = pipelam_reset_window_timeout(window, current_timeout_id, config->window_timeout);
    } else {
        int index = pipelam_find_window_index(window);
        if (index >= 0) {
            window_list[index].timeout_id = pipelam_reset_window_timeout(window, window_list[index].timeout_id, config->window_timeout);
        }
    }

    gtk_window_present(window);
}
// Main render functions for each type
static void pipelam_render_wob_window(GtkApplication *app, gpointer ptr_pipelam_config) {
    struct pipelam_config *pipelam_config = (struct pipelam_config *)ptr_pipelam_config;

    int percentage = 0;
    if (pipelam_config->expression != NULL) {
        percentage = atoi(pipelam_config->expression);
        if (percentage < 0)
            percentage = 0;
        if (percentage > 100)
            percentage = 100;
    }
    pipelam_log_debug("WOB value: %d%%", percentage);

    if (pipelam_config->runtime_behaviour != OVERLAY) {
        if (current_window != NULL && GTK_IS_WIDGET(bar_fg)) {
            pipelam_log_debug("Updating existing WOB window");

            int bar_width = (350 * percentage) / 100;
            gtk_widget_set_size_request(bar_fg, bar_width, 25);

            pipelam_update_window_settings(current_window, pipelam_config);
            if (current_timeout_id > 0) {
                g_source_remove(current_timeout_id);
            }
            current_timeout_id = g_timeout_add(pipelam_config->window_timeout, close_window_callback, current_window);
            pipelam_log_debug("Reset timeout (ID: %u)", current_timeout_id);

            gtk_window_present(current_window);
            return;
        }
    } else if (current_window != NULL && GTK_IS_WIDGET(bar_fg)) {
        pipelam_log_debug("Updating existing WOB window");

        int bar_width = (350 * percentage) / 100;
        gtk_widget_set_size_request(bar_fg, bar_width, 25);

        pipelam_update_window_settings(current_window, pipelam_config);

        if (current_timeout_id > 0) {
            g_source_remove(current_timeout_id);
        }
        current_timeout_id = g_timeout_add(pipelam_config->window_timeout, close_window_callback, current_window);
        pipelam_log_debug("Reset timeout (ID: %u)", current_timeout_id);

        gtk_window_present(current_window);
        return;
    }

    pipelam_log_debug("Creating new WOB window");
    GtkWindow *gtk_window = pipelam_render_gtk_window(app, pipelam_config);
    if (gtk_window == NULL) {
        pipelam_log_error("gtk_render_gtk_window() returned NULL");
        return;
    }

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *bar_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    GtkWidget *bar_bg = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(bar_bg, TRUE);
    gtk_widget_set_size_request(bar_bg, 350, 25);

    GtkWidget *new_bar_fg = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    int bar_width = (350 * percentage) / 100;
    gtk_widget_set_size_request(new_bar_fg, bar_width, 25);

    GtkWidget *border_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    gtk_widget_add_css_class(border_container, "wob-border");
    gtk_widget_add_css_class(bar_bg, "wob-background");
    gtk_widget_add_css_class(new_bar_fg, "wob-foreground");
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css_data = ".wob-border { background-color: white; padding: 4px; margin: 4px; border: 4px solid black; }"
                           ".wob-background { background-color: black; padding: 4px; }"
                           ".wob-foreground { background-color: white; padding: 4px; }";

    gtk_css_provider_load_from_string(provider, css_data);

    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_box_append(GTK_BOX(bar_bg), new_bar_fg);
    gtk_box_append(GTK_BOX(border_container), bar_bg);
    gtk_box_append(GTK_BOX(bar_container), border_container);
    gtk_box_append(GTK_BOX(box), bar_container);

    gtk_window_set_child(gtk_window, box);

    pipelam_log_debug("window_timeout: %d", pipelam_config->window_timeout);
    guint timeout_id = g_timeout_add(pipelam_config->window_timeout, close_window_callback, gtk_window);
    pipelam_log_debug("Set new timeout (ID: %u)", timeout_id);

    if (pipelam_config->runtime_behaviour == OVERLAY) {
        pipelam_add_window_to_list(gtk_window, WOB, timeout_id, new_bar_fg);
        pipelam_log_debug("Added WOB window to overlay list");
    } else {
        if (current_window != NULL) {
            pipelam_log_debug("Closing previous window of different type");
            gtk_window_close(current_window);
        }

        current_window = gtk_window;
        current_window_type = WOB;
        current_timeout_id = timeout_id;
        bar_fg = new_bar_fg;
    }

    gtk_window_present(gtk_window);
    g_object_unref(provider);
}

static void pipelam_render_image_window(GtkApplication *app, gpointer ptr_pipelam_config) {
    pipelam_log_debug("Handling image window");
    struct pipelam_config *pipelam_config = (struct pipelam_config *)ptr_pipelam_config;

    const char *image_path = pipelam_config->expression;

    if (pipelam_config->runtime_behaviour != OVERLAY) {
        if (current_window != NULL && current_window_type == IMAGE) {
            pipelam_update_image_window(current_window, image_path, pipelam_config);
            return;
        }
    } else if (current_window != NULL && current_window_type == IMAGE) {
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

    guint timeout_id = pipelam_reset_window_timeout(gtk_window, 0, pipelam_config->window_timeout);

    if (pipelam_config->runtime_behaviour == OVERLAY) {
        pipelam_add_window_to_list(gtk_window, IMAGE, timeout_id, NULL);
        pipelam_log_debug("Added IMAGE window to overlay list");
    } else {
        if (pipelam_config->runtime_behaviour == REPLACE && current_window != NULL && current_window_type != IMAGE) {
            pipelam_log_debug("Closing previous window of different type");
            gtk_window_close(current_window);
        }

        current_window = gtk_window;
        current_window_type = IMAGE;
        current_timeout_id = timeout_id;
    }

    gtk_window_present(gtk_window);
}

static void pipelam_render_text_window(GtkApplication *app, gpointer ptr_pipelam_config) {
    pipelam_log_debug("Handling text window");
    struct pipelam_config *pipelam_config = (struct pipelam_config *)ptr_pipelam_config;

    if (pipelam_config->runtime_behaviour != OVERLAY) {
        if (current_window != NULL && current_window_type == TEXT) {
            pipelam_update_text_window(current_window, pipelam_config->expression, pipelam_config);
            return;
        }
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

    guint timeout_id = pipelam_reset_window_timeout(gtk_window, 0, pipelam_config->window_timeout);

    if (pipelam_config->runtime_behaviour == OVERLAY) {
        pipelam_add_window_to_list(gtk_window, TEXT, timeout_id, NULL);
        pipelam_log_debug("Added TEXT window to overlay list");
    } else {
        // If we're replacing a window and it's a different type, close the old one
        if (pipelam_config->runtime_behaviour == REPLACE && current_window != NULL && current_window_type != TEXT) {
            pipelam_log_debug("Closing previous window of different type");
            gtk_window_close(current_window);
        }

        current_window = gtk_window;
        current_window_type = TEXT;
        current_timeout_id = timeout_id;
    }

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

    if (is_updating_window) {
        pipelam_log_debug("Window update already in progress, queueing");
        g_timeout_add(10, pipelam_create_window, ptr_pipelam_config);
        return FALSE;
    }

    is_updating_window = TRUE;

    if (app == NULL) {
        pipelam_log_error("No GTK application available");
        is_updating_window = FALSE;
        return FALSE;
    }

    if (pipelam_config->runtime_behaviour != OVERLAY) {
        current_window_type = pipelam_config->type;
    }

    if (pipelam_config->runtime_behaviour == QUEUE && (current_window != NULL || window_count > 0)) {
        g_timeout_add(10, pipelam_create_window, ptr_pipelam_config);
        is_updating_window = FALSE;
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

    is_updating_window = FALSE;

    // Clean up the saved expression after window creation is complete
    // This happens after the window is created, not when this function returns
    if (saved_config.expression != NULL && saved_config.expression != pipelam_config->expression) {
        g_free(saved_config.expression);
        saved_config.expression = NULL;
    }

    return G_SOURCE_REMOVE;
}
