#include "../src/config.h"
#include "../src/log.h"
#include "../src/message.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Test parsing a plain text message
static void test_parse_plain_text() {
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
static void test_parse_json_text() {
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
static void test_parse_json_image() {
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
static void test_parse_invalid_json() {
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
static void test_reset_to_defaults() {
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

    pipelam_log_test("=== All Message Tests Passed ===");
    return 0;
}
