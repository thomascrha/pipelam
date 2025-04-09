#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../src/config.h"
#include "../src/log.h"

extern char *create_temp_config_file(const char *content);
extern void cleanup_temp_file(char *filename);

static void test_cmdline_override_config_file(void) {
    pipelam_log_test("Testing command line options override config file...");

    // Create config file with settings
    const char *config_content = "log_level = INFO\n"
                                 "runtime_behaviour = queue\n"
                                 "window_timeout = 1000\n"
                                 "anchor = bottom-right\n"
                                 "margin_left = 50\n"
                                 "margin_right = 60\n"
                                 "margin_top = 70\n"
                                 "margin_bottom = 80\n";

    char *temp_file = create_temp_config_file(config_content);
    assert(temp_file != NULL);

    // Ensure no environment variables affect our test
    unsetenv("PIPELAM_LOG_LEVEL");
    unsetenv("PIPELAM_RUNTIME_BEHAVIOUR");
    unsetenv("PIPELAM_WINDOW_TIMEOUT");
    unsetenv("PIPELAM_ANCHOR");
    unsetenv("PIPELAM_MARGIN_LEFT");
    unsetenv("PIPELAM_MARGIN_RIGHT");
    unsetenv("PIPELAM_MARGIN_TOP");
    unsetenv("PIPELAM_MARGIN_BOTTOM");

    // First load the config with only the config file
    struct pipelam_config *config = pipelam_setup_config(temp_file);

    // Verify config values from file
    assert(config != NULL);
    assert(strcmp(config->log_level, "INFO") == 0);
    assert(config->runtime_behaviour == QUEUE);
    assert(config->window_timeout == 1000);
    assert(config->anchor == BOTTOM_RIGHT);
    assert(config->margin_left == 50);
    assert(config->margin_right == 60);
    assert(config->margin_top == 70);
    assert(config->margin_bottom == 80);

    // Now simulate command line arguments
    char *argv[] = {"pipelam",
                    "--log-level=DEBUG",
                    "--runtime-behaviour=replace",
                    "--window-timeout=2000",
                    "--anchor=top-left",
                    "--margin-left=100",
                    "--margin-right=110",
                    "--margin-top=120",
                    "--margin-bottom=130",
                    "/path/to/pipe"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    // Process command line arguments to override settings
    pipelam_process_command_line_args(argc, argv, config);

    // Check if command line options override config file settings
    assert(strcmp(config->log_level, "DEBUG") == 0);
    assert(config->runtime_behaviour == REPLACE);
    assert(config->window_timeout == 2000);
    assert(config->anchor == TOP_LEFT);
    assert(config->margin_left == 100);
    assert(config->margin_right == 110);
    assert(config->margin_top == 120);
    assert(config->margin_bottom == 130);

    pipelam_destroy_config(config);
    cleanup_temp_file(temp_file);
    pipelam_log_test("Command line options override config file test: PASSED");
}

static void test_cmdline_override_environment(void) {
    pipelam_log_test("Testing command line options override environment variables...");

    // Set environment variables
    setenv("PIPELAM_LOG_LEVEL", "WARNING", 1);
    setenv("PIPELAM_RUNTIME_BEHAVIOUR", "overlay", 1);
    setenv("PIPELAM_WINDOW_TIMEOUT", "3000", 1);
    setenv("PIPELAM_ANCHOR", "CENTER", 1);
    setenv("PIPELAM_MARGIN_LEFT", "150", 1);
    setenv("PIPELAM_MARGIN_RIGHT", "160", 1);
    setenv("PIPELAM_MARGIN_TOP", "170", 1);
    setenv("PIPELAM_MARGIN_BOTTOM", "180", 1);

    // Load config with environment variables
    struct pipelam_config *config = pipelam_setup_config(NULL);

    // Verify config values from environment
    assert(config != NULL);
    assert(strcmp(config->log_level, "WARNING") == 0);
    assert(config->runtime_behaviour == OVERLAY);
    assert(config->window_timeout == 3000);
    assert(config->anchor == CENTER);
    assert(config->margin_left == 150);
    assert(config->margin_right == 160);
    assert(config->margin_top == 170);
    assert(config->margin_bottom == 180);

    // Now simulate command line arguments
    char *argv[] = {"pipelam",
                    "--log-level=DEBUG",
                    "--runtime-behaviour=replace",
                    "--window-timeout=2000",
                    "--anchor=top-left",
                    "--margin-left=100",
                    "--margin-right=110",
                    "--margin-top=120",
                    "--margin-bottom=130",
                    "/path/to/pipe"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    // Process command line arguments to override environment variables
    pipelam_process_command_line_args(argc, argv, config);

    // Check if command line options override environment variable settings
    assert(strcmp(config->log_level, "DEBUG") == 0);
    assert(config->runtime_behaviour == REPLACE);
    assert(config->window_timeout == 2000);
    assert(config->anchor == TOP_LEFT);
    assert(config->margin_left == 100);
    assert(config->margin_right == 110);
    assert(config->margin_top == 120);
    assert(config->margin_bottom == 130);

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

    pipelam_log_test("Command line options override environment variables test: PASSED");
}

static void test_cmdline_override_both(void) {
    pipelam_log_test("Testing command line options override both config file and environment variables...");

    // Create config file with settings
    const char *config_content = "log_level = INFO\n"
                                 "runtime_behaviour = queue\n"
                                 "window_timeout = 1000\n"
                                 "anchor = bottom-right\n"
                                 "margin_left = 50\n"
                                 "margin_right = 60\n"
                                 "margin_top = 70\n"
                                 "margin_bottom = 80\n";

    char *temp_file = create_temp_config_file(config_content);
    assert(temp_file != NULL);

    // Set environment variables with different values
    setenv("PIPELAM_LOG_LEVEL", "WARNING", 1);
    setenv("PIPELAM_RUNTIME_BEHAVIOUR", "overlay", 1);
    setenv("PIPELAM_WINDOW_TIMEOUT", "3000", 1);
    setenv("PIPELAM_ANCHOR", "CENTER", 1);
    setenv("PIPELAM_MARGIN_LEFT", "150", 1);
    setenv("PIPELAM_MARGIN_RIGHT", "160", 1);
    setenv("PIPELAM_MARGIN_TOP", "170", 1);
    setenv("PIPELAM_MARGIN_BOTTOM", "180", 1);

    // Load config (this will have environment values which override config file)
    struct pipelam_config *config = pipelam_setup_config(temp_file);

    // Verify config values from environment (since env overrides config file)
    assert(config != NULL);
    assert(strcmp(config->log_level, "WARNING") == 0);
    assert(config->runtime_behaviour == OVERLAY);
    assert(config->window_timeout == 3000);
    assert(config->anchor == CENTER);
    assert(config->margin_left == 150);
    assert(config->margin_right == 160);
    assert(config->margin_top == 170);
    assert(config->margin_bottom == 180);

    // Now simulate command line arguments with yet different values
    char *argv[] = {"pipelam",
                    "--log-level=DEBUG",
                    "--runtime-behaviour=replace",
                    "--window-timeout=2000",
                    "--anchor=top-left",
                    "--margin-left=100",
                    "--margin-right=110",
                    "--margin-top=120",
                    "--margin-bottom=130",
                    "/path/to/pipe"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    // Process command line arguments
    pipelam_process_command_line_args(argc, argv, config);

    // Check if command line options override both config file and environment settings
    assert(strcmp(config->log_level, "DEBUG") == 0);
    assert(config->runtime_behaviour == REPLACE);
    assert(config->window_timeout == 2000);
    assert(config->anchor == TOP_LEFT);
    assert(config->margin_left == 100);
    assert(config->margin_right == 110);
    assert(config->margin_top == 120);
    assert(config->margin_bottom == 130);

    pipelam_destroy_config(config);
    cleanup_temp_file(temp_file);

    // Clean up environment
    unsetenv("PIPELAM_LOG_LEVEL");
    unsetenv("PIPELAM_RUNTIME_BEHAVIOUR");
    unsetenv("PIPELAM_WINDOW_TIMEOUT");
    unsetenv("PIPELAM_ANCHOR");
    unsetenv("PIPELAM_MARGIN_LEFT");
    unsetenv("PIPELAM_MARGIN_RIGHT");
    unsetenv("PIPELAM_MARGIN_TOP");
    unsetenv("PIPELAM_MARGIN_BOTTOM");

    pipelam_log_test("Command line options override both config file and environment variables test: PASSED");
}

static void test_cmdline_short_options(void) {
    pipelam_log_test("Testing command line short options...");

    struct pipelam_config *config = pipelam_setup_config(NULL);

    // Reset to known values
    config->log_level = "INFO";
    config->runtime_behaviour = QUEUE;
    config->window_timeout = 1000;
    config->anchor = CENTER;
    config->margin_left = 50;
    config->margin_right = 60;
    config->margin_top = 70;
    config->margin_bottom = 80;

    // Test with short options
    char *argv[] = {"pipelam", "-l", "DEBUG", "-r", "replace", "-t", "2000", "-a", "top-right", "-L", "100", "-R", "110", "-T", "120", "-B", "130", "/path/to/pipe"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    pipelam_process_command_line_args(argc, argv, config);

    // Verify short options worked correctly
    assert(strcmp(config->log_level, "DEBUG") == 0);
    assert(config->runtime_behaviour == REPLACE);
    assert(config->window_timeout == 2000);
    assert(config->anchor == TOP_RIGHT);
    assert(config->margin_left == 100);
    assert(config->margin_right == 110);
    assert(config->margin_top == 120);
    assert(config->margin_bottom == 130);

    pipelam_destroy_config(config);
    pipelam_log_test("Command line short options test: PASSED");
}

static void test_cmdline_partial_options(void) {
    pipelam_log_test("Testing partial command line options...");

    struct pipelam_config *config = pipelam_setup_config(NULL);

    // Reset to known values
    config->log_level = "INFO";
    config->runtime_behaviour = QUEUE;
    config->window_timeout = 1000;
    config->anchor = CENTER;
    config->margin_left = 50;
    config->margin_right = 60;
    config->margin_top = 70;
    config->margin_bottom = 80;

    // Only specify some options
    char *argv[] = {"pipelam", "--log-level=DEBUG", "--margin-top=120", "/path/to/pipe"};
    int argc = sizeof(argv) / sizeof(argv[0]);

    pipelam_process_command_line_args(argc, argv, config);

    // Verify only specified options were changed
    assert(strcmp(config->log_level, "DEBUG") == 0); // Changed
    assert(config->runtime_behaviour == QUEUE);      // Unchanged
    assert(config->window_timeout == 1000);          // Unchanged
    assert(config->anchor == CENTER);                // Unchanged
    assert(config->margin_left == 50);               // Unchanged
    assert(config->margin_right == 60);              // Unchanged
    assert(config->margin_top == 120);               // Changed
    assert(config->margin_bottom == 80);             // Unchanged

    pipelam_destroy_config(config);
    pipelam_log_test("Partial command line options test: PASSED");
}

static void test_cmdline_version_option(void) {
    pipelam_log_test("Testing version command line option...");

    const char *executable = "./build/pipelam";
    char cmd_buffer[256];
    char output_buffer[1024];
    FILE *fp;
    int result;

    pipelam_log_test("Expected version from config.h: %s", PIPELAM_CURRENT_VERSION);

    // Test short version option (-v)
    snprintf(cmd_buffer, sizeof(cmd_buffer), "%s -v", executable);
    pipelam_log_test("Running command: %s", cmd_buffer);

    fp = popen(cmd_buffer, "r");
    assert(fp != NULL);

    // Read output
    memset(output_buffer, 0, sizeof(output_buffer));
    while (fgets(output_buffer + strlen(output_buffer), sizeof(output_buffer) - strlen(output_buffer) - 1, fp) != NULL)
        ;

    result = pclose(fp);
    assert(WIFEXITED(result) && WEXITSTATUS(result) == 0);

    // Check that output contains the version from config.h
    pipelam_log_test("Short option output: %s", output_buffer);
    assert(strstr(output_buffer, PIPELAM_CURRENT_VERSION) != NULL);

    // Test long version option (--version)
    snprintf(cmd_buffer, sizeof(cmd_buffer), "%s --version", executable);
    pipelam_log_test("Running command: %s", cmd_buffer);

    fp = popen(cmd_buffer, "r");
    assert(fp != NULL);

    // Read output
    memset(output_buffer, 0, sizeof(output_buffer));
    while (fgets(output_buffer + strlen(output_buffer), sizeof(output_buffer) - strlen(output_buffer) - 1, fp) != NULL)
        ;

    result = pclose(fp);
    assert(WIFEXITED(result) && WEXITSTATUS(result) == 0);

    // Check that output contains the version from config.h
    pipelam_log_test("Long option output: %s", output_buffer);
    assert(strstr(output_buffer, PIPELAM_CURRENT_VERSION) != NULL);

    pipelam_log_test("Version command line option test: PASSED");
}

int test_cmdline_options_main(void) {
    pipelam_log_level_set(LOG_DEBUG);

    pipelam_log_test("=== Starting Command Line Options Tests ===");

    test_cmdline_override_config_file();
    test_cmdline_override_environment();
    test_cmdline_override_both();
    test_cmdline_short_options();
    test_cmdline_partial_options();
    pipelam_log_level_set(LOG_DEBUG);
    test_cmdline_version_option();

    pipelam_log_test("=== Starting Command Line Options Tests ===");
    pipelam_log_test("=== All Command Line Options Tests Passed ===");
    return 0;
}
