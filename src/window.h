#ifndef WINDOW_H
#define WINDOW_H
#include "config.h"

int bow_create_run_window(char *volume_expression, int window_timeout);
void bow_set_anchor(enum bow_window_anchor anchor);

#endif // WINDOW_H

