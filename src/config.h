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

struct bow_config {
	int buffer_size;
	int log_level;
	int window_timeout;
};

struct bow_config* bow_setup_config(void);
void bow_destroy_config(struct bow_config *config);
int bow_log_level_set_from_string(const char *log_level);

#endif // CONFIG_H
