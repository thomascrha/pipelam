#ifndef CONFIG_H
#define CONFIG_H

#include <gtk/gtk.h>

#define RED "\033[0;31m"
#define BOLD_RED "\033[1;31m"
#define YELLOW "\033[0;33m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"

enum pipelam_window_anchor {
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	TOP_LEFT,
	TOP_RIGHT,
	CENTER
};

enum pipelam_message_type {
	TEXT,
	IMAGE
};

struct pipelam_config {
	int buffer_size;
	char *log_level;
	int window_timeout;
	char *expression;
	enum pipelam_message_type type;
	enum pipelam_window_anchor anchor;
	int margin_left;
	int margin_right;
	int margin_top;
	int margin_bottom;
};

gpointer *pipelam_setup_config(void);
void pipelam_destroy_config(struct pipelam_config *config);

// taken from, altered slightly https://github.com/welljsjs/Config-Parser-C/blob/master/parser.h
#define CONFIG_ARG_MAX_BYTES 128

typedef struct pipelam_config_option pipelam_config_option;
typedef pipelam_config_option* pipelam_config_option_t;

struct pipelam_config_option {
    pipelam_config_option_t prev;
    char key[CONFIG_ARG_MAX_BYTES];
    char value[CONFIG_ARG_MAX_BYTES];
};

#endif // CONFIG_H
