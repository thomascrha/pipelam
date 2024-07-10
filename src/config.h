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

#define BUFFER_SIZE 2048
#define WINDOW_TIMEOUT 800

extern int buffer_size;
extern int log_level;
extern int window_timeout;

#endif // CONFIG_H
