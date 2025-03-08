#ifndef WINDOW_H
#define WINDOW_H
#include "glib.h"

void bow_create_run_window(gpointer bow_config);
void bow_close_current_window(void);
int bow_has_active_window(void);

#endif // WINDOW_H


