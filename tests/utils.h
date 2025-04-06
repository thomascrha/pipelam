#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef PIPELAM_UTILS_H
#define PIPELAM_UTILS_H

char* create_temp_config_file(const char* content);
void cleanup_temp_file(char* filename);

#endif /* PIPELAM_UTILS_H */

#ifdef UTILS_IMPLEMENTATION
#ifndef UTILS_IMPLEMENTATION_DEFINED
#define UTILS_IMPLEMENTATION_DEFINED

char* create_temp_config_file(const char* content) {
    char* filename = strdup("/tmp/pipelam_test_cmdline_XXXXXX");
    int fd = mkstemp(filename);
    if (fd < 0) {
        perror("Failed to create temp file");
        free(filename);
        return NULL;
    }

    if (content) {
        write(fd, content, strlen(content));
    }
    close(fd);

    return filename;
}

void cleanup_temp_file(char* filename) {
    if (filename) {
        unlink(filename);
        free(filename);
    }
}

#endif /* UTILS_IMPLEMENTATION_DEFINED */
#endif /* UTILS_IMPLEMENTATION */
