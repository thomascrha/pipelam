#ifndef WINDOW_H
#define WINDOW_H
#include "glib.h"

void pipelam_create_run_window(gpointer pipelam_config);
void pipelam_close_current_window(void);
int pipelam_has_active_window(void);

#endif // WINDOW_H


