#ifndef CONFIG_H
#define CONFIG_H

#define LOG_DEBUG 0
#define LOG_INFO 1
#define LOG_WARNING 2
#define LOG_ERROR 3
#define LOG_PANIC 4

#define RED "\033[0;31m"
#define BOLD_RED "\033[1;31m"
#define YELLOW "\033[0;33m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"

enum bow_window_anchor {
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	TOP_LEFT,
	TOP_RIGHT,
	CENTER
};

struct bow_config {
	int buffer_size;
	int log_level;
	int window_timeout;
	char *volume_expression;
	enum bow_window_anchor anchor;
	int margin_left;
	int margin_right;
	int margin_top;
	int margin_bottom;
};

struct bow_config* bow_setup_config(void);
void bow_destroy_config(struct bow_config *config);

#endif // CONFIG_H
