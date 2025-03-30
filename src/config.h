#ifndef CONFIG_H
#define CONFIG_H

#include <gtk/gtk.h>

#define RED "\033[0;31m"
#define BOLD_RED "\033[1;31m"
#define YELLOW "\033[0;33m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"
#define PURPLE "\033[0;35m"

// These are fallback defaults if no config is provided
#define FALLBACK_WINDOW_TIMEOUT 600
#define FALLBACK_ANCHOR CENTER
#define FALLBACK_MARGIN_LEFT 100
#define FALLBACK_MARGIN_RIGHT 0
#define FALLBACK_MARGIN_TOP 100
#define FALLBACK_MARGIN_BOTTOM 0
#define FALLBACK_LOG_LEVEL "INFO"
#define FALLBACK_RUNTIME_BEHAVIOUR OVERLAY

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

enum pipelam_runtime_behaviour {
	QUEUE,
	REPLACE,
	OVERLAY
};

struct pipelam_config {
	// Startrup behaviour
	char *log_level;
	enum pipelam_runtime_behaviour runtime_behaviour;

	// Message behaviour
	char *expression;
	enum pipelam_message_type type;

	// Runtime behaviour
	int window_timeout;
	enum pipelam_window_anchor anchor;
	int margin_left;
	int margin_right;
	int margin_top;
	int margin_bottom;

	// Default values (set at startup from config/env)
	int default_window_timeout;
	enum pipelam_window_anchor default_anchor;
	int default_margin_left;
	int default_margin_right;
	int default_margin_top;
	int default_margin_bottom;
};

struct pipelam_config *pipelam_setup_config(const char *config_file_path);
void pipelam_destroy_config(struct pipelam_config *config);
void pipelam_reset_default_config(struct pipelam_config *config);

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
