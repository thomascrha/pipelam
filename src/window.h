#ifndef WINDOW_H
#define WINDOW_H
#include "config.h"
#include "glib.h"

int bow_create_run_window(gpointer bow_config);
void bow_set_anchor(enum bow_window_anchor anchor);

#endif // WINDOW_H

