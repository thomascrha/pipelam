#ifndef LOG_H
#define LOG_H

#define LOG_DEBUG   0
#define LOG_INFO    1
#define LOG_WARNING 2
#define LOG_ERROR   3
#define LOG_PANIC   4
#define LOG_TEST    5

void pipelam_log_level_set(int log_level);
void pipelam_log_message(int log_level, const char *file, int line, ...);

#define pipelam_log_debug(...) pipelam_log_message(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

#define pipelam_log_info(...) pipelam_log_message(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)

#define pipelam_log_warning(...) pipelam_log_message(LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__)

#define pipelam_log_error(...) pipelam_log_message(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#define pipelam_log_test(...) pipelam_log_message(LOG_TEST, __FILE__, __LINE__, __VA_ARGS__)

#define pipelam_log_panic(...)  \
	pipelam_log_message(LOG_PANIC, __FILE__, __LINE__, __VA_ARGS__);  \
	exit(2)

#endif // LOG_H
