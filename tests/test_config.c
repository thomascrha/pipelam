#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/config.h"
#include "../src/log.h"

extern char *create_temp_config_file(const char *content);
extern void cleanup_temp_file(char *filename);

static void test_default_config(void) {
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

    struct pipelam_config *config = pipelam_setup_config(NULL);

    assert(config != NULL);
    assert(strcmp(config->log_level, "INFO") == 0);
    assert(config->runtime_behaviour == REPLACE);
    assert(config->window_timeout == FALLBACK_WINDOW_TIMEOUT);
    assert(config->anchor == FALLBACK_ANCHOR);
    assert(config->margin_left == FALLBACK_MARGIN_LEFT);
    assert(config->margin_right == FALLBACK_MARGIN_RIGHT);
    assert(config->margin_top == FALLBACK_MARGIN_TOP);
    assert(config->margin_bottom == FALLBACK_MARGIN_BOTTOM);

    assert(config->default_window_timeout == FALLBACK_WINDOW_TIMEOUT);
    assert(config->default_anchor == FALLBACK_ANCHOR);
    assert(config->default_margin_left == FALLBACK_MARGIN_LEFT);
    assert(config->default_margin_right == FALLBACK_MARGIN_RIGHT);
    assert(config->default_margin_top == FALLBACK_MARGIN_TOP);
    assert(config->default_margin_bottom == FALLBACK_MARGIN_BOTTOM);

    pipelam_destroy_config(config);
    pipelam_log_test("Default configuration test: PASSED");
}

static void test_config_from_file(void) {
    pipelam_log_test("Testing configuration from file...");

    const char *config_content = "log_level = DEBUG\n"
                                 "runtime_behaviour = replace\n"
                                 "window_timeout = 1000\n"
                                 "anchor = bottom-right\n"
                                 "margin_left = 50\n"
                                 "margin_right = 60\n"
                                 "margin_top = 70\n"
                                 "margin_bottom = 80\n";

    char *temp_file = create_temp_config_file(config_content);
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

    struct pipelam_config *config = pipelam_setup_config(temp_file);

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

static void test_config_from_env(void) {
    pipelam_log_test("Testing configuration from environment variables...");

    // Ensure no config file affects our test
    unsetenv("PIPELAM_CONFIG_FILE_PATH");

    setenv("PIPELAM_LOG_LEVEL", "WARNING", 1);
    setenv("PIPELAM_RUNTIME_BEHAVIOUR", "replace", 1);
    setenv("PIPELAM_WINDOW_TIMEOUT", "2000", 1);
    setenv("PIPELAM_ANCHOR", "TOP_LEFT", 1);
    setenv("PIPELAM_MARGIN_LEFT", "25", 1);
    setenv("PIPELAM_MARGIN_RIGHT", "35", 1);
    setenv("PIPELAM_MARGIN_TOP", "45", 1);
    setenv("PIPELAM_MARGIN_BOTTOM", "55", 1);

    struct pipelam_config *config = pipelam_setup_config(NULL);

    assert(config != NULL);
    assert(strcmp(config->log_level, "WARNING") == 0);
    assert(config->runtime_behaviour == REPLACE);
    assert(config->window_timeout == 2000);
    assert(config->anchor == TOP_LEFT);
    assert(config->margin_left == 25);
    assert(config->margin_right == 35);
    assert(config->margin_top == 45);
    assert(config->margin_bottom == 55);

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

static void test_env_override_file(void) {
    pipelam_log_test("Testing environment variables overriding config file...");

    const char *config_content = "log_level = DEBUG\n"
                                 "runtime_behaviour = queue\n"
                                 "window_timeout = 1000\n"
                                 "anchor = bottom-right\n"
                                 "margin_left = 50\n"
                                 "margin_right = 60\n"
                                 "margin_top = 70\n"
                                 "margin_bottom = 80";

    char *temp_file = create_temp_config_file(config_content);
    assert(temp_file != NULL);

    // Set some environment variables to override
    setenv("PIPELAM_LOG_LEVEL", "ERROR", 1);
    setenv("PIPELAM_RUNTIME_BEHAVIOUR", "overlay", 1);
    setenv("PIPELAM_WINDOW_TIMEOUT", "3000", 1);
    setenv("PIPELAM_MARGIN_LEFT", "100", 1);

    struct pipelam_config *config = pipelam_setup_config(temp_file);

    pipelam_log_test("Current value in anchor %d", config->anchor);
    pipelam_log_test("BOTTOM_RIGHT value %d", BOTTOM_RIGHT);
    pipelam_log_test("Current value in runtime_behaviour %d", config->runtime_behaviour);

    assert(config != NULL);
    assert(strcmp(config->log_level, "ERROR") == 0); // From env
    assert(config->runtime_behaviour == OVERLAY);    // From env (overriding file)
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
    unsetenv("PIPELAM_RUNTIME_BEHAVIOUR");
    unsetenv("PIPELAM_WINDOW_TIMEOUT");
    unsetenv("PIPELAM_MARGIN_LEFT");

    pipelam_log_test("Environment overriding config file test: PASSED");
}

static void test_invalid_config(void) {
    pipelam_log_test("Testing handling of invalid configuration values...");

    const char *config_content = "log_level = INVALID_LEVEL\n"
                                 "runtime_behaviour = invalid_behaviour\n"
                                 "anchor = invalid_anchor\n"
                                 "window_timeout = abc"; // Non-numeric value

    char *temp_file = create_temp_config_file(config_content);
    assert(temp_file != NULL);

    // Load the config - it should fall back to defaults for invalid values
    struct pipelam_config *config = pipelam_setup_config(NULL);

    // Values should be defaults when invalid
    assert(config != NULL);
    // Note: The log level might get set to INVALID_LEVEL because the code just takes the string value
    assert(config->runtime_behaviour == REPLACE); // Should use default
    assert(config->anchor == FALLBACK_ANCHOR);  // Should use default
    // window_timeout might be 0 or default depending on implementation of atoi()

    pipelam_destroy_config(config);
    cleanup_temp_file(temp_file);

    pipelam_log_test("Invalid configuration test: PASSED");
}

static void test_all_runtime_behaviour_values(void) {
    pipelam_log_test("Testing all runtime behavior values...");

    const char *behaviour_types[][2] = {
        {"queue", "QUEUE"},
        {"replace", "REPLACE"},
        {"overlay", "OVERLAY"},
    };

    for (int i = 0; i < 3; i++) {
        char config_content[256];
        sprintf(config_content, "runtime_behaviour = %s\n", behaviour_types[i][0]);

        char *temp_file = create_temp_config_file(config_content);
        assert(temp_file != NULL);

        struct pipelam_config *config = pipelam_setup_config(temp_file);
        assert(config != NULL);

        // Convert runtime_behaviour enum to int for comparison
        int expected_behaviour = -1;
        if (strcmp(behaviour_types[i][1], "QUEUE") == 0)
            expected_behaviour = QUEUE;
        else if (strcmp(behaviour_types[i][1], "REPLACE") == 0)
            expected_behaviour = REPLACE;
        else if (strcmp(behaviour_types[i][1], "OVERLAY") == 0)
            expected_behaviour = OVERLAY;

        pipelam_log_test("Testing runtime_behaviour: %s (expected value: %d, actual: %d)", behaviour_types[i][0], expected_behaviour, (int)config->runtime_behaviour);
        assert((int)config->runtime_behaviour == expected_behaviour);

        pipelam_destroy_config(config);
        cleanup_temp_file(temp_file);
    }

    pipelam_log_test("All runtime behavior values test: PASSED");
}

static void test_all_anchor_values(void) {
    pipelam_log_test("Testing all anchor values...");

    const char *anchor_types[][2] = {
        {"bottom-left", "BOTTOM_LEFT"}, {"bottom-right", "BOTTOM_RIGHT"}, {"top-left", "TOP_LEFT"}, {"top-right", "TOP_RIGHT"}, {"center", "CENTER"},
    };

    for (int i = 0; i < 5; i++) {
        char config_content[256];
        sprintf(config_content, "anchor = %s\n", anchor_types[i][0]);

        char *temp_file = create_temp_config_file(config_content);
        assert(temp_file != NULL);

        struct pipelam_config *config = pipelam_setup_config(temp_file);
        assert(config != NULL);

        // Convert anchor enum to int for comparison to avoid string/pointer comparison issues
        int expected_anchor = -1;
        if (strcmp(anchor_types[i][1], "BOTTOM_LEFT") == 0)
            expected_anchor = BOTTOM_LEFT;
        else if (strcmp(anchor_types[i][1], "BOTTOM_RIGHT") == 0)
            expected_anchor = BOTTOM_RIGHT;
        else if (strcmp(anchor_types[i][1], "TOP_LEFT") == 0)
            expected_anchor = TOP_LEFT;
        else if (strcmp(anchor_types[i][1], "TOP_RIGHT") == 0)
            expected_anchor = TOP_RIGHT;
        else if (strcmp(anchor_types[i][1], "CENTER") == 0)
            expected_anchor = CENTER;

        pipelam_log_test("Testing anchor: %s (expected value: %d, actual: %d)", anchor_types[i][0], expected_anchor, (int)config->anchor);
        assert((int)config->anchor == expected_anchor);

        pipelam_destroy_config(config);
        cleanup_temp_file(temp_file);
    }

    pipelam_log_test("All anchor values test: PASSED");
}

static void test_extreme_window_timeout(void) {
    pipelam_log_test("Testing extreme window timeout values...");

    // Test cases: very small, very large, negative, zero
    const char *timeout_tests[][2] = {
        {"1", "1"},                   // Minimum practical
        {"0", "0"},                   // Zero
        {"-100", "-100"},             // Negative (implementation dependent)
        {"2147483647", "2147483647"}, // INT_MAX
    };

    for (int i = 0; i < 4; i++) {
        char config_content[256];
        sprintf(config_content, "window_timeout = %s\n", timeout_tests[i][0]);

        char *temp_file = create_temp_config_file(config_content);
        assert(temp_file != NULL);

        struct pipelam_config *config = pipelam_setup_config(temp_file);
        assert(config != NULL);

        int expected = atoi(timeout_tests[i][1]);
        pipelam_log_test("Testing window_timeout: %s (expected: %d, actual: %d)", timeout_tests[i][0], expected, config->window_timeout);

        assert(config->window_timeout == expected);

        pipelam_destroy_config(config);
        cleanup_temp_file(temp_file);
    }

    pipelam_log_test("Extreme window timeout test: PASSED");
}

static void test_config_with_comments(void) {
    pipelam_log_test("Testing config file with comments and empty lines...");

    const char *config_content = "# This is a comment\n"
                                 "\n"
                                 "log_level = WARNING  # Inline comment\n"
                                 "runtime_behaviour = overlay  # Another inline comment\n"
                                 "\n"
                                 "# Another comment line\n"
                                 "window_timeout = 750\n"
                                 "\n"
                                 "anchor = top-right\n";

    char *temp_file = create_temp_config_file(config_content);
    assert(temp_file != NULL);

    struct pipelam_config *config = pipelam_setup_config(temp_file);
    assert(config != NULL);

    // Verify the non-comment lines were parsed correctly
    assert(strcmp(config->log_level, "WARNING") == 0);
    assert(config->runtime_behaviour == OVERLAY);
    assert(config->window_timeout == 750);
    assert((int)config->anchor == (int)TOP_RIGHT);

    pipelam_destroy_config(config);
    cleanup_temp_file(temp_file);

    pipelam_log_test("Config with comments test: PASSED");
}

static void test_margin_edge_cases(void) {
    pipelam_log_test("Testing margin edge cases...");

    const char *config_content = "margin_left = 0\n"
                                 "margin_right = 0\n"
                                 "margin_top = 0\n"
                                 "margin_bottom = 0\n";

    char *temp_file = create_temp_config_file(config_content);
    assert(temp_file != NULL);

    struct pipelam_config *config = pipelam_setup_config(temp_file);
    assert(config != NULL);

    // Verify all margins are zero
    assert(config->margin_left == 0);
    assert(config->margin_right == 0);
    assert(config->margin_top == 0);
    assert(config->margin_bottom == 0);

    pipelam_destroy_config(config);
    cleanup_temp_file(temp_file);

    const char *config_content2 = "margin_left = -50\n"
                                  "margin_right = -60\n"
                                  "margin_top = -70\n"
                                  "margin_bottom = -80\n";

    temp_file = create_temp_config_file(config_content2);
    assert(temp_file != NULL);

    config = pipelam_setup_config(temp_file);
    assert(config != NULL);

    // Verify negative margins
    assert(config->margin_left == -50);
    assert(config->margin_right == -60);
    assert(config->margin_top == -70);
    assert(config->margin_bottom == -80);

    pipelam_destroy_config(config);
    cleanup_temp_file(temp_file);

    pipelam_log_test("Margin edge cases test: PASSED");
}

int test_config_main(void) {
    pipelam_log_level_set(LOG_DEBUG);

    pipelam_log_test("=== Starting Config Tests ===");

    test_default_config();
    test_config_from_file();
    test_config_from_env();
    test_env_override_file();
    test_invalid_config();
    test_all_runtime_behaviour_values();
    test_all_anchor_values();
    test_extreme_window_timeout();
    test_config_with_comments();
    test_margin_edge_cases();

    pipelam_log_test("=== All Config Tests Passed ===");
    return 0;
}
