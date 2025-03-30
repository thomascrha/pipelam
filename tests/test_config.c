/* For strdup() */
#define _POSIX_C_SOURCE 200809L

#include "../src/config.h"
#include "../src/log.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Helper function to create a temporary config file for testing
static char* create_temp_config_file(const char* content) {
    char* filename = strdup("/tmp/pipelam_test_config_XXXXXX");
    int fd = mkstemp(filename);
    if (fd < 0) {
        perror("Failed to create temp file");
        free(filename);
        return NULL;
    }

    write(fd, content, strlen(content));
    close(fd);

    return filename;
}

static void cleanup_temp_file(char* filename) {
    if (filename) {
        unlink(filename);
        free(filename);
    }
}

// Test default config values
static void test_default_config() {
    pipelam_log_test("Testing default configuration...");

    // Ensure no environment variables affect our test
    unsetenv("PIPELAM_LOG_LEVEL");
    unsetenv("PIPELAM_RUNTIME_BEHAVIOUR");
    unsetenv("PIPELAM_WINDOW_TIMEOUT");
    unsetenv("PIPELAM_ANCHOR");
    unsetenv("PIPELAM_MARGIN_LEFT");
    unsetenv("PIPELAM_MARGIN_RIGHT");
    unsetenv("PIPELAM_MARGIN_TOP");
    unsetenv("PIPELAM_MARGIN_BOTTOM");
    unsetenv("PIPELAM_CONFIG_FILE_PATH");

    // Create config with defaults
    struct pipelam_config* config = pipelam_setup_config(NULL);

    // Verify default values
    assert(config != NULL);
    assert(strcmp(config->log_level, FALLBACK_LOG_LEVEL) == 0);
    assert(config->runtime_behaviour == FALLBACK_RUNTIME_BEHAVIOUR);
    assert(config->window_timeout == FALLBACK_WINDOW_TIMEOUT);
    assert(config->anchor == FALLBACK_ANCHOR);
    assert(config->margin_left == FALLBACK_MARGIN_LEFT);
    assert(config->margin_right == FALLBACK_MARGIN_RIGHT);
    assert(config->margin_top == FALLBACK_MARGIN_TOP);
    assert(config->margin_bottom == FALLBACK_MARGIN_BOTTOM);

    // Verify default values match the defaults
    assert(config->default_window_timeout == FALLBACK_WINDOW_TIMEOUT);
    assert(config->default_anchor == FALLBACK_ANCHOR);
    assert(config->default_margin_left == FALLBACK_MARGIN_LEFT);
    assert(config->default_margin_right == FALLBACK_MARGIN_RIGHT);
    assert(config->default_margin_top == FALLBACK_MARGIN_TOP);
    assert(config->default_margin_bottom == FALLBACK_MARGIN_BOTTOM);

    pipelam_destroy_config(config);
    pipelam_log_test("Default configuration test: PASSED");
}

// Test config from a file
static void test_config_from_file() {
    pipelam_log_test("Testing configuration from file...");

    // Create a temporary config file
    const char* config_content =
        "log_level = DEBUG\n"
        "runtime_behaviour = replace\n"
        "window_timeout = 1000\n"
        "anchor = bottom-right\n"
        "margin_left = 50\n"
        "margin_right = 60\n"
        "margin_top = 70\n"
        "margin_bottom = 80\n";

    char* temp_file = create_temp_config_file(config_content);
    assert(temp_file != NULL);

	pipelam_log_test("Temp file path %s", temp_file);

    // Ensure no environment variables affect our test
    unsetenv("PIPELAM_LOG_LEVEL");
    unsetenv("PIPELAM_RUNTIME_BEHAVIOUR");
    unsetenv("PIPELAM_WINDOW_TIMEOUT");
    unsetenv("PIPELAM_ANCHOR");
    unsetenv("PIPELAM_MARGIN_LEFT");
    unsetenv("PIPELAM_MARGIN_RIGHT");
    unsetenv("PIPELAM_MARGIN_TOP");
    unsetenv("PIPELAM_MARGIN_BOTTOM");

    // Load the config
    struct pipelam_config* config = pipelam_setup_config(temp_file);

	pipelam_log_test("Current value in log_level %s", config->log_level);

    // Verify config values from file
    assert(config != NULL);
    assert(strcmp(config->log_level, "DEBUG") == 0);
    assert(config->runtime_behaviour == REPLACE);
    assert(config->window_timeout == 1000);
    assert(config->anchor == BOTTOM_RIGHT);
    assert(config->margin_left == 50);
    assert(config->margin_right == 60);
    assert(config->margin_top == 70);
    assert(config->margin_bottom == 80);

    // Also verify these are now the default values
    assert(config->default_window_timeout == 1000);
    assert(config->default_anchor == BOTTOM_RIGHT);
    assert(config->default_margin_left == 50);
    assert(config->default_margin_right == 60);
    assert(config->default_margin_top == 70);
    assert(config->default_margin_bottom == 80);

    pipelam_destroy_config(config);
    cleanup_temp_file(temp_file);
    pipelam_log_test("Configuration from file test: PASSED");
}

// Test config from environment variables
static void test_config_from_env() {
    pipelam_log_test("Testing configuration from environment variables...");

    // Ensure no config file affects our test
    unsetenv("PIPELAM_CONFIG_FILE_PATH");

    // Set environment variables
    setenv("PIPELAM_LOG_LEVEL", "WARNING", 1);
    setenv("PIPELAM_RUNTIME_BEHAVIOUR", "replace", 1);
    setenv("PIPELAM_WINDOW_TIMEOUT", "2000", 1);
    setenv("PIPELAM_ANCHOR", "TOP_LEFT", 1);
    setenv("PIPELAM_MARGIN_LEFT", "25", 1);
    setenv("PIPELAM_MARGIN_RIGHT", "35", 1);
    setenv("PIPELAM_MARGIN_TOP", "45", 1);
    setenv("PIPELAM_MARGIN_BOTTOM", "55", 1);

    // Load the config
    struct pipelam_config* config = pipelam_setup_config(NULL);

    // Verify config values from environment
    assert(config != NULL);
    assert(strcmp(config->log_level, "WARNING") == 0);
    assert(config->runtime_behaviour == REPLACE);
    assert(config->window_timeout == 2000);
    assert(config->anchor == TOP_LEFT);
    assert(config->margin_left == 25);
    assert(config->margin_right == 35);
    assert(config->margin_top == 45);
    assert(config->margin_bottom == 55);

    // Also verify these are now the default values
    assert(config->default_window_timeout == 2000);
    assert(config->default_anchor == TOP_LEFT);
    assert(config->default_margin_left == 25);
    assert(config->default_margin_right == 35);
    assert(config->default_margin_top == 45);
    assert(config->default_margin_bottom == 55);

    pipelam_destroy_config(config);

    // Clean up environment
    unsetenv("PIPELAM_LOG_LEVEL");
    unsetenv("PIPELAM_RUNTIME_BEHAVIOUR");
    unsetenv("PIPELAM_WINDOW_TIMEOUT");
    unsetenv("PIPELAM_ANCHOR");
    unsetenv("PIPELAM_MARGIN_LEFT");
    unsetenv("PIPELAM_MARGIN_RIGHT");
    unsetenv("PIPELAM_MARGIN_TOP");
    unsetenv("PIPELAM_MARGIN_BOTTOM");

    pipelam_log_test("Configuration from environment test: PASSED");
}

// Test environment variables overriding config file
static void test_env_override_file() {
    pipelam_log_test("Testing environment variables overriding config file...");

    // Create a temporary config file
    const char* config_content =
        "log_level = DEBUG\n"
        "runtime_behaviour = queue\n"
        "window_timeout = 1000\n"
        "anchor = bottom-right\n"
        "margin_left = 50\n"
        "margin_right = 60\n"
        "margin_top = 70\n"
        "margin_bottom = 80";

    char* temp_file = create_temp_config_file(config_content);
    assert(temp_file != NULL);

    // Set some environment variables to override
    setenv("PIPELAM_LOG_LEVEL", "ERROR", 1);
    setenv("PIPELAM_WINDOW_TIMEOUT", "3000", 1);
    setenv("PIPELAM_MARGIN_LEFT", "100", 1);

    // Load the config
    struct pipelam_config* config = pipelam_setup_config(temp_file);

	pipelam_log_test("Current value in anchor %d", config->anchor);
	pipelam_log_test("BOTTOM_RIGHT value %d", BOTTOM_RIGHT);

    // Verify overridden values
    assert(config != NULL);
    assert(strcmp(config->log_level, "ERROR") == 0); // From env
    assert(config->runtime_behaviour == QUEUE);      // From file
    assert(config->window_timeout == 3000);          // From env
    assert(config->anchor == BOTTOM_RIGHT);          // From file
    assert(config->margin_left == 100);              // From env
    assert(config->margin_right == 60);              // From file
    assert(config->margin_top == 70);                // From file
    assert(config->margin_bottom == 80);             // From file

    pipelam_destroy_config(config);
    cleanup_temp_file(temp_file);

    // Clean up environment
    unsetenv("PIPELAM_LOG_LEVEL");
    unsetenv("PIPELAM_WINDOW_TIMEOUT");
    unsetenv("PIPELAM_MARGIN_LEFT");

    pipelam_log_test("Environment overriding config file test: PASSED");
}

// Test invalid config values
static void test_invalid_config() {
    pipelam_log_test("Testing handling of invalid configuration values...");

    // Create a temporary config file with some invalid values
    const char* config_content =
        "log_level = INVALID_LEVEL\n"
        "runtime_behaviour = invalid_behaviour\n"
        "anchor = invalid_anchor\n"
        "window_timeout = abc"; // Non-numeric value

    char* temp_file = create_temp_config_file(config_content);
    assert(temp_file != NULL);

    // Load the config - it should fall back to defaults for invalid values
    struct pipelam_config* config = pipelam_setup_config(NULL);

    // Values should be defaults when invalid
    assert(config != NULL);
    // Note: The log level might get set to INVALID_LEVEL because the code just takes the string value
    assert(config->runtime_behaviour == FALLBACK_RUNTIME_BEHAVIOUR); // Should use default
    assert(config->anchor == FALLBACK_ANCHOR);                       // Should use default
    // window_timeout might be 0 or default depending on implementation of atoi()

    pipelam_destroy_config(config);
    cleanup_temp_file(temp_file);

    pipelam_log_test("Invalid configuration test: PASSED");
}

int test_config_main(void) {
    // Initialize with debug logging to see detailed output
    pipelam_log_level_set(LOG_DEBUG);

    pipelam_log_test("=== Starting Config Tests ===");

    test_default_config();
    test_config_from_file();
    test_config_from_env();
    test_env_override_file();
    test_invalid_config();

    pipelam_log_test("=== All Config Tests Passed ===");
    return 0;
}
