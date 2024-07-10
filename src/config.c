#include "config.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

static void
bow_log_level_set_from_string (const char *log_level)
{
    if (strcmp (log_level, "DEBUG") == 0)
        {
            bow_log_level_set (LOG_DEBUG);
        }
    else if (strcmp (log_level, "INFO") == 0)
        {
            bow_log_level_set (LOG_INFO);
        }
    else if (strcmp (log_level, "WARNING") == 0)
        {
            bow_log_level_set (LOG_WARNING);
        }
    else if (strcmp (log_level, "ERROR") == 0)
        {
            bow_log_level_set (LOG_ERROR);
        }
    else if (strcmp (log_level, "PANIC") == 0)
        {
            bow_log_level_set (LOG_PANIC);
        }
    else
        {
            bow_log_error ("Unknown log level: %s", log_level);
        }
}

void
destroy_bow_config (struct bow_config *config)
{
    free (config);
}

struct bow_config *
bow_setup_config (void)
{
    struct bow_config *config = malloc (sizeof (struct bow_config));
    if (config == NULL)
        {
            bow_log_error ("Failed to allocate memory for bow_config");
            return NULL;
        }

    config->buffer_size = 2048;
    config->log_level = 1;
    config->window_timeout = 600;

    const char *log_level_env = getenv ("BOW_LOG_LEVEL");
    if (log_level_env == NULL)
        {
            log_level_env = "INFO";
        }
    bow_log_level_set_from_string (log_level_env);

    const char *buffer_size_env = getenv ("BOW_BUFFER_SIZE");
    if (buffer_size_env != NULL)
        {
            config->buffer_size = atoi (buffer_size_env);
        }

    const char *window_timeout_env = getenv ("BOW_WINDOW_TIMEOUT");
    if (window_timeout_env != NULL)
        {
            config->window_timeout = atoi (window_timeout_env);
        }

    return config;
}
