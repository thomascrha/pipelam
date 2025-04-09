#define _POSIX_C_SOURCE 200809L
#include "../src/log.h"
#include <stdlib.h>

extern int test_config_main(void);
extern int test_message_main(void);
extern int test_window_main(void);
extern int test_cmdline_options_main(void);

int main(void) {
    // Set environment variable to skip default config files
    setenv("PIPELAM_SKIP_DEFAULT_CONFIG", "1", 1);

    // Set log level to debug for detailed test output
    pipelam_log_level_set(LOG_DEBUG);

    pipelam_log_test("=======================================");
    pipelam_log_test("       PIPELAM TEST SUITE RUNNER       ");
    pipelam_log_test("=======================================");

    int tests_failed = 0;

    // Run config tests
    pipelam_log_test("[Running Config Tests]");
    pipelam_log_test("---------------------------------------");
    if (test_config_main() != 0) {
        tests_failed++;
        pipelam_log_test("CONFIG TESTS FAILED!");
    } else {
        pipelam_log_test("CONFIG TESTS PASSED!");
    }

    // Run message tests
    pipelam_log_test("[Running Message Tests]");
    pipelam_log_test("---------------------------------------");
    if (test_message_main() != 0) {
        tests_failed++;
        pipelam_log_test("MESSAGE TESTS FAILED!");
    } else {
        pipelam_log_test("MESSAGE TESTS PASSED!");
    }

    // Run command line options tests
    pipelam_log_test("[Running Command Line Options Tests]");
    pipelam_log_test("---------------------------------------");
    if (test_cmdline_options_main() != 0) {
        tests_failed++;
        pipelam_log_test("COMMAND LINE OPTIONS TESTS FAILED!");
    } else {
        pipelam_log_test("COMMAND LINE OPTIONS TESTS PASSED!");
    }

    // Summary
    pipelam_log_test("=======================================");
    if (tests_failed > 0) {
        pipelam_log_test("OVERALL RESULT: %d TEST GROUPS FAILED!", tests_failed);
        return 1;
    } else {
        pipelam_log_test("OVERALL RESULT: ALL TESTS PASSED!");
        return 0;
    }
}
