#ifndef WINDOW_H
#define WINDOW_H
#include <gtk/gtk.h>

void pipelam_create_window(gpointer pipelam_config);
void pipelam_close_current_window(void);
void pipelam_set_application(GtkApplication *app);

#endif // WINDOW_H


