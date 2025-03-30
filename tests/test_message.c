#include "../src/config.h"
#include "../src/log.h"
#include "../src/message.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test parsing a plain text message
static void test_parse_plain_text(void) {
    pipelam_log_test("Testing plain text message parsing...");

    // Create a config
    struct pipelam_config config;

    // Set some initial values that should be overridden
    config.window_timeout = 1000;
    config.anchor = TOP_RIGHT;
    config.margin_left = 10;
    config.margin_right = 20;
    config.margin_top = 30;
    config.margin_bottom = 40;

    // Set defaults
    config.default_window_timeout = 1000;
    config.default_anchor = TOP_RIGHT;
    config.default_margin_left = 10;
    config.default_margin_right = 20;
    config.default_margin_top = 30;
    config.default_margin_bottom = 40;

    // Parse a plain text message
    const char* message = "This is a plain text message";
    pipelam_parse_message(message, &config);

    // Verify the message was parsed correctly
    assert(config.expression != NULL);
    assert(strcmp(config.expression, message) == 0);
    assert(config.type == TEXT);

    // Verify the settings were unchanged
    assert(config.window_timeout == 1000);
    assert(config.anchor == TOP_RIGHT);
    assert(config.margin_left == 10);
    assert(config.margin_right == 20);
    assert(config.margin_top == 30);
    assert(config.margin_bottom == 40);

    pipelam_log_test("Plain text message parsing test: PASSED");
}

// Test parsing a JSON message with text content
static void test_parse_json_text(void) {
    pipelam_log_test("Testing JSON text message parsing...");

    // Create a config
    struct pipelam_config config;

    // Set some initial values
    config.window_timeout = 1000;
    config.anchor = TOP_RIGHT;
    config.margin_left = 10;
    config.margin_right = 20;
    config.margin_top = 30;
    config.margin_bottom = 40;

    // Set defaults
    config.default_window_timeout = 1000;
    config.default_anchor = TOP_RIGHT;
    config.default_margin_left = 10;
    config.default_margin_right = 20;
    config.default_margin_top = 30;
    config.default_margin_bottom = 40;

    // Parse a JSON message with text content
    const char* json_message = "{\"type\":\"text\",\"expression\":\"This is a JSON text message\",\"settings\":{\"window_timeout\":2000,\"anchor\":\"bottom-left\",\"margin_left\":50,\"margin_right\":60,\"margin_top\":70,\"margin_bottom\":80}}";
    pipelam_parse_message(json_message, &config);

    // Verify the message was parsed correctly
    assert(config.expression != NULL);
    assert(strcmp(config.expression, "This is a JSON text message") == 0);
    assert(config.type == TEXT);

    // Verify the settings were updated
    assert(config.window_timeout == 2000);
    assert(config.anchor == BOTTOM_LEFT);
    assert(config.margin_left == 50);
    assert(config.margin_right == 60);
    assert(config.margin_top == 70);
    assert(config.margin_bottom == 80);

    pipelam_log_test("JSON text message parsing test: PASSED");
}

// Test parsing a JSON message with image content
static void test_parse_json_image(void) {
    pipelam_log_test("Testing JSON image message parsing...");

    // Create a config
    struct pipelam_config config;

    // Set some initial values
    config.window_timeout = 1000;
    config.anchor = TOP_RIGHT;
    config.margin_left = 10;
    config.margin_right = 20;
    config.margin_top = 30;
    config.margin_bottom = 40;

    // Set defaults
    config.default_window_timeout = 1000;
    config.default_anchor = TOP_RIGHT;
    config.default_margin_left = 10;
    config.default_margin_right = 20;
    config.default_margin_top = 30;
    config.default_margin_bottom = 40;

    // Parse a JSON message with image content
    const char* json_message = "{\"type\":\"image\",\"expression\":\"/path/to/image.png\",\"settings\":{\"window_timeout\":3000,\"anchor\":\"center\"}}";
    pipelam_parse_message(json_message, &config);

    // Verify the message was parsed correctly
    assert(config.expression != NULL);
    assert(strcmp(config.expression, "/path/to/image.png") == 0);
    assert(config.type == IMAGE);

    // Verify only specified settings were updated
    assert(config.window_timeout == 3000);
    assert(config.anchor == CENTER);
    assert(config.margin_left == 10);  // Unchanged
    assert(config.margin_right == 20); // Unchanged
    assert(config.margin_top == 30);   // Unchanged
    assert(config.margin_bottom == 40); // Unchanged

    pipelam_log_test("JSON image message parsing test: PASSED");
}

// Test parsing an invalid JSON message
static void test_parse_invalid_json(void) {
    pipelam_log_test("Testing invalid JSON message parsing...");

    // Create a config
    struct pipelam_config config;

    // Set some initial values
    config.window_timeout = 1000;
    config.anchor = TOP_RIGHT;
    config.margin_left = 10;
    config.margin_right = 20;
    config.margin_top = 30;
    config.margin_bottom = 40;

    // Set defaults
    config.default_window_timeout = 1000;
    config.default_anchor = TOP_RIGHT;
    config.default_margin_left = 10;
    config.default_margin_right = 20;
    config.default_margin_top = 30;
    config.default_margin_bottom = 40;

    // Parse an invalid JSON message
    const char* invalid_json = "{\"type\":\"text\",\"expression\":\"Broken JSON";
    pipelam_parse_message(invalid_json, &config);

    // For invalid JSON, it should treat it as plain text
    assert(config.expression != NULL);
    assert(strcmp(config.expression, invalid_json) == 0);
    assert(config.type == TEXT);

    pipelam_log_test("Invalid JSON message parsing test: PASSED");
}

// Test resetting to defaults before parsing
static void test_reset_to_defaults(void) {
    pipelam_log_test("Testing reset to defaults before parsing...");

    // Create a config
    struct pipelam_config config;

    // Set custom defaults
    config.default_window_timeout = 5000;
    config.default_anchor = BOTTOM_RIGHT;
    config.default_margin_left = 100;
    config.default_margin_right = 200;
    config.default_margin_top = 300;
    config.default_margin_bottom = 400;

    // Set current values different from defaults
    config.window_timeout = 1000;
    config.anchor = TOP_LEFT;
    config.margin_left = 10;
    config.margin_right = 20;
    config.margin_top = 30;
    config.margin_bottom = 40;

    // Parse a plain message (which should reset to defaults)
    const char* message = "This should reset values to defaults";
    pipelam_parse_message(message, &config);

    // Verify that values were reset to defaults
    assert(config.window_timeout == 5000);
    assert(config.anchor == BOTTOM_RIGHT);
    assert(config.margin_left == 100);
    assert(config.margin_right == 200);
    assert(config.margin_top == 300);
    assert(config.margin_bottom == 400);

    pipelam_log_test("Reset to defaults test: PASSED");
}

// Test partial JSON settings (only some settings specified)
static void test_partial_json_settings(void) {
    pipelam_log_test("Testing partial JSON settings...");

    // Create a config
    struct pipelam_config config;

    // Set initial values
    config.window_timeout = 1000;
    config.anchor = TOP_RIGHT;
    config.margin_left = 10;
    config.margin_right = 20;
    config.margin_top = 30;
    config.margin_bottom = 40;

    // Set defaults
    config.default_window_timeout = 1000;
    config.default_anchor = TOP_RIGHT;
    config.default_margin_left = 10;
    config.default_margin_right = 20;
    config.default_margin_top = 30;
    config.default_margin_bottom = 40;

    // Parse a JSON with only some settings specified
    const char* json_message = "{\"type\":\"text\",\"expression\":\"Partial settings test\",\"settings\":{\"window_timeout\":5000,\"margin_top\":100}}";
    pipelam_parse_message(json_message, &config);

    // Verify only the specified settings were changed
    assert(config.window_timeout == 5000);  // Changed
    assert(config.anchor == TOP_RIGHT);     // Unchanged
    assert(config.margin_left == 10);       // Unchanged
    assert(config.margin_right == 20);      // Unchanged
    assert(config.margin_top == 100);       // Changed
    assert(config.margin_bottom == 40);     // Unchanged

    pipelam_log_test("Partial JSON settings test: PASSED");
}

// Test empty JSON message
static void test_empty_json_message(void) {
    pipelam_log_test("Testing empty JSON message...");

    // Create a config
    struct pipelam_config config;

    // Set initial values
    config.window_timeout = 1000;
    config.anchor = TOP_RIGHT;
    config.default_window_timeout = 1000;
    config.default_anchor = TOP_RIGHT;

    // Parse an empty JSON message
    const char* json_message = "{}";
    pipelam_parse_message(json_message, &config);

    // Verify the result - should fallback to defaults and be treated as text
    assert(config.expression != NULL);
    assert(strcmp(config.expression, "{}") == 0);
    assert(config.type == TEXT);  // Should default to TEXT

    pipelam_log_test("Empty JSON message test: PASSED");
}

// Test all anchor types in JSON
static void test_all_json_anchors(void) {
    pipelam_log_test("Testing all anchor types in JSON...");

    // Create a config
    struct pipelam_config config;

    // Test data: anchor string in JSON and expected enum value
    const char* anchor_tests[][2] = {
        {"bottom-left", "BOTTOM_LEFT"},
        {"bottom-right", "BOTTOM_RIGHT"},
        {"top-left", "TOP_LEFT"},
        {"top-right", "TOP_RIGHT"},
        {"center", "CENTER"}
    };

    for (int i = 0; i < 5; i++) {
        // Reset config
        config.window_timeout = 1000;
        config.anchor = TOP_RIGHT;  // Default to something else
        config.default_window_timeout = 1000;
        config.default_anchor = TOP_RIGHT;

        // Create JSON with this anchor
        char json_message[256];
        sprintf(json_message, "{\"type\":\"text\",\"expression\":\"Test\",\"settings\":{\"anchor\":\"%s\"}}",
                anchor_tests[i][0]);

        pipelam_parse_message(json_message, &config);

        // Convert expected anchor enum to int for comparison
        int expected_anchor = -1;
        if (strcmp(anchor_tests[i][1], "BOTTOM_LEFT") == 0) expected_anchor = BOTTOM_LEFT;
        else if (strcmp(anchor_tests[i][1], "BOTTOM_RIGHT") == 0) expected_anchor = BOTTOM_RIGHT;
        else if (strcmp(anchor_tests[i][1], "TOP_LEFT") == 0) expected_anchor = TOP_LEFT;
        else if (strcmp(anchor_tests[i][1], "TOP_RIGHT") == 0) expected_anchor = TOP_RIGHT;
        else if (strcmp(anchor_tests[i][1], "CENTER") == 0) expected_anchor = CENTER;

        pipelam_log_test("Testing JSON anchor: %s (expected: %d, actual: %d)",
                        anchor_tests[i][0], expected_anchor, (int)config.anchor);

        assert((int)config.anchor == expected_anchor);
    }

    pipelam_log_test("All JSON anchors test: PASSED");
}

// Test malformed JSON with valid structure
static void test_malformed_valid_json(void) {
    pipelam_log_test("Testing malformed but valid JSON...");

    // Create a config
    struct pipelam_config config;

    // Set initial values
    config.window_timeout = 1000;
    config.anchor = TOP_RIGHT;
    config.default_window_timeout = 1000;
    config.default_anchor = TOP_RIGHT;

    // Parse JSON with invalid field names but valid structure
    const char* json_message = "{\"unknown_field\":\"value\",\"another_field\":123}";
    pipelam_parse_message(json_message, &config);

    // Should be treated as TEXT since required fields are missing
    assert(config.expression != NULL);
    assert(strcmp(config.expression, json_message) == 0);
    assert(config.type == TEXT);

    pipelam_log_test("Malformed valid JSON test: PASSED");
}

// Test extremely long text message
static void test_long_text_message(void) {
    pipelam_log_test("Testing extremely long text message...");

    // Create a config
    struct pipelam_config config;

    // Set initial values
    config.window_timeout = 1000;
    config.anchor = TOP_RIGHT;
    config.default_window_timeout = 1000;
    config.default_anchor = TOP_RIGHT;

    // Create a very long message (1000+ characters)
    char long_message[1100];
    memset(long_message, 'A', sizeof(long_message) - 1);
    long_message[sizeof(long_message) - 1] = '\0';

    pipelam_parse_message(long_message, &config);

    // Verify the long message was handled correctly
    assert(config.expression != NULL);
    assert(strcmp(config.expression, long_message) == 0);
    assert(config.type == TEXT);

    pipelam_log_test("Long text message test: PASSED");
}

// Main test function for message tests
int test_message_main(void) {
    // Initialize with debug logging to see detailed output
    pipelam_log_level_set(LOG_DEBUG);

    pipelam_log_test("=== Starting Message Tests ===");

    test_parse_plain_text();
    test_parse_json_text();
    test_parse_json_image();
    test_parse_invalid_json();
    test_reset_to_defaults();

    // New test cases
    test_partial_json_settings();
    test_empty_json_message();
    test_all_json_anchors();
    test_malformed_valid_json();
    test_long_text_message();

    pipelam_log_test("=== All Message Tests Passed ===");
    return 0;
}
