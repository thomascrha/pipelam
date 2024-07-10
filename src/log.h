#ifndef LOG_H
#define LOG_H

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

void bow_log_message(int importance, const char *file, int line, ...);
void bow_log_set_level(int importance);

#define bow_log_debug(...) bow_log_message(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

#define bow_log_info(...) bow_log_message(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)

#define bow_log_warning(...) bow_log_message(LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__)

#define bow_log_error(...) bow_log_message(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#define bow_log_panic(...)  \
	bow_log_message(LOG_PANIC, __FILE__, __LINE__, __VA_ARGS__);  \
	exit(2)

#endif // LOG_H
