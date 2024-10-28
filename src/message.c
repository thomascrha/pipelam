
#include "config.h"
#include "json.h"
#include "log.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static enum bow_window_anchor bow_json_config_anchor_parse(struct json_string_s *value) {
    if (0 == strcmp(value->string, "bottom-left")) {
        return BOTTOM_LEFT;
    } else if (0 == strcmp(value->string, "bottom-right")) {
        return BOTTOM_RIGHT;
    } else if (0 == strcmp(value->string, "top-left")) {
        return TOP_LEFT;
    } else if (0 == strcmp(value->string, "top-right")) {
        return TOP_RIGHT;
    } else if (0 == strcmp(value->string, "center")) {
        return CENTER;
    } else {
        bow_log_error("Unknown anchor: %s", value->string);
        return CENTER;
    }
}

static void bow_json_config_settings_parse(struct json_object_s *object, struct bow_config *config) {
    char *keys[] = {"window_timeout", "anchor", "margin_left", "margin_right", "margin_top", "margin_bottom"};

    struct json_object_element_s *element = object->start;
    printf("bow_json_config_settings_parse\n");
    while (element != NULL) {
        struct json_string_s *name = element->name;
        struct json_value_s *value = element->value;

        for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
            char *key = keys[i];
            if (0 == strcmp(name->string, key)) {
                printf("key: %s\n", key);
                if (0 == strcmp(name->string, "window_timeout")) {
                    struct json_number_s *_value = json_value_as_number(value);
                    if (_value != NULL) {
                        printf("window_timeout: %s\n", _value->number);
                        config->window_timeout = atoi(_value->number);
                        printf("window_timeout: %d\n", config->window_timeout);
                    }

                } else if (0 == strcmp(name->string, "anchor")) {
                    struct json_string_s *_value = json_value_as_string(value);
                    printf("anchor: %s\n", _value->string);
                    if (_value != NULL) {
                        config->anchor = bow_json_config_anchor_parse(_value);
                    }

                } else if (0 == strcmp(name->string, "margin_left")) {
                    struct json_number_s *_value = json_value_as_number(value);
                    if (_value != NULL) {
                        config->margin_left = atoi(_value->number);
                    }

                } else if (0 == strcmp(name->string, "margin_right")) {
                    struct json_number_s *_value = json_value_as_number(value);
                    if (_value != NULL) {
                        config->margin_right = atoi(_value->number);
                    }

                } else if (0 == strcmp(name->string, "margin_top")) {
                    struct json_number_s *_value = json_value_as_number(value);
                    if (_value != NULL) {
                        config->margin_top = atoi(_value->number);
                    }

                } else if (0 == strcmp(name->string, "margin_bottom")) {
                    struct json_number_s *_value = json_value_as_number(value);
                    if (_value != NULL) {
                        config->margin_bottom = atoi(_value->number);
                    }

                } else {
                    printf("unknown key: %s\n", name->string);
                }
            }
        }
        element = element->next;
    }
    bow_log_debug("window_timeout: %d", config->window_timeout);
}

static void bow_json_config_parse(struct json_object_s *object, struct bow_config *config) {
    bow_log_info("bow_json_config_parse");
    char *keys[] = {"expression", "type", "settings"};

    struct json_object_element_s *element = object->start;
    while (element != NULL) {
        struct json_string_s *name = element->name;
        struct json_value_s *value = element->value;

        for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
            char *key = keys[i];

            if (0 == strcmp(name->string, key)) {
                if (0 == strcmp(name->string, "expression")) {
                    bow_log_debug("expression");
                    struct json_string_s *_value = json_value_as_string(value);
                    bow_log_debug("expression: %s", (char *)_value->string);
                    if (_value != NULL) {
                        config->expression = (char *)_value->string;
                    } else {
                        bow_log_error("expression is NULL");
                        config->expression = NULL;
                    }

                } else if (0 == strcmp(name->string, "type")) {
                    struct json_string_s *_value = json_value_as_string(value);
                    bow_log_debug("type: %s", (char *)_value->string);
                    if (_value != NULL) {
                        if (0 == strcmp(_value->string, "text")) {
                            config->type = TEXT;
                        } else if (0 == strcmp(_value->string, "image")) {
                            config->type = IMAGE;
                        } else {
                            bow_log_error("Unknown type: %s", _value->string);
                            config->type = TEXT;
                        }
                    }

                } else if (0 == strcmp(name->string, "settings")) {
                    bow_json_config_settings_parse(json_value_as_object(value), config);
                } else {
                    printf("unknown key: %s\n", name->string);
                }
            }
        }

        element = element->next;
    }
}

void bow_parse_message(const char *expression, struct bow_config *config) {
    // check if first char of expression is '{'
    // I know this is dumb, but its proably the best way in this instance to check
    // parsing only occurs with an object not a list of objects, and if the the first
    // char is not '{' then we can assume its a string
    bow_log_debug("bow parse string");
    if (expression[0] != '{') {
        bow_log_info("Treating as string %s", expression);
        config->expression = (char *)expression;
        config->type = TEXT;
        return;
    }

    bow_log_debug("bow parse json");
    struct json_value_s *root = json_parse(expression, strlen(expression));
    if (root == NULL) {
        bow_log_error("Json not parsable, Invalid JSON: %s", expression);
        config->expression = (char *)expression;
        config->type = TEXT;
        return;
    }

    bow_log_debug("bow parse json object");
    struct json_object_s *object = json_value_as_object(root);
    bow_log_debug("bow parse json object");
    if (object == NULL) {
        bow_log_error("No root object found, Invalid JSON: %s", expression);
        config->expression = (char *)expression;
        config->type = TEXT;
        return;
    }

    bow_log_debug("bow_json_config_parse");
    bow_json_config_parse(object, config);
}
