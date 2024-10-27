
#include "config.h"
#include "json.h"
#include "log.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// static struct bow_json_config_settings *bow_json_config_settings_parse(struct json_object_s* object) {
// 	struct bow_json_config_settings* settings = malloc(sizeof(struct bow_json_config_settings));
// 	char* keys[] = {"window_timeout", "volume_expression", "anchor", "margin_left", "margin_right", "margin_top", "margin_bottom"};
//
// 	struct json_object_element_s* element = object->start;
// 	printf("bow_json_config_settings_parse\n");
// 	while (element != NULL) {
// 		struct json_string_s* name = element->name;
// 		struct json_value_s* value = element->value;
// 		// struct json_object_s* value_object = json_value_as_object(value); // Use a different variable
// 		struct json_string_s* value_string = json_value_as_string(value);
//
// 		for (size_t i = 0; i < sizeof(keys)/sizeof(keys[0]); i++) {
// 			char* key = keys[i];
//
// 			printf("key: %s\n", key);
// 			printf("name: %s\n", name->string);
// 			printf("value: %s\n", json_value_as_number(value)->number);
// 			if (0 == strcmp(name->string, key)) {
// 				printf("key: %s\n", key);
// 				if (0 == strcmp(name->string, "window_timeout")) {
// 					printf("here window_timeout\n");
// 					settings->window_timeout = atoi(json_value_as_number(value)->number);
// 					// settings->window_timeout = json_value_as_number(value)->number;
// 					printf("window_timeout: %d\n", settings->window_timeout);
// 				} else if (0 == strcmp(name->string, "volume_expression")) {
// 					settings->volume_expression = (char*)value_string->string;
//
// 				} else if (0 == strcmp(name->string, "anchor")) {
// 					settings->anchor = (enum bow_window_anchor)atoi(value_string->string);
//
// 				} else if (0 == strcmp(name->string, "margin_left")) {
// 					settings->margin_left = atoi(value_string->string);
//
// 				} else if (0 == strcmp(name->string, "margin_right")) {
// 					settings->margin_right = atoi(value_string->string);
//
// 				} else if (0 == strcmp(name->string, "margin_top")) {
// 					settings->margin_top = atoi(value_string->string);
//
// 				} else if (0 == strcmp(name->string, "margin_bottom")) {
// 					settings->margin_bottom = atoi(value_string->string);
//
// 				} else {
// 					printf("unknown key: %s\n", name->string);
// 				}
// 			}
// 		}
//
// 		element = element->next;
// 	}
//
// 	return settings;
// }

static void bow_json_config_parse(struct json_object_s *object, struct bow_config *config) {
    char *keys[] = {"expression", "type", "settings"};

    struct json_object_element_s *element = object->start;
    while (element != NULL) {
        struct json_string_s *name = element->name;
        struct json_value_s *value = element->value;

        for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
            char *key = keys[i];

            if (0 == strcmp(name->string, key)) {
                if (0 == strcmp(name->string, "expression")) {
                    struct json_string_s *_value = json_value_as_string(value);
                    if (_value != NULL) {
                        config->expression = (char *)_value;
                    } else {
                        bow_log_error("expression is NULL");
                        config->expression = NULL;
                    }

                } else if (0 == strcmp(name->string, "type")) {
                    struct json_string_s *_value = json_value_as_string(value);
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
                    printf("settings unsupported %s\n", (char *)value->payload);
                    // config->settings = bow_json_config_settings_parse(json_value_as_object(value));

                } else {
                    printf("unknown key: %s\n", name->string);
                }
            }
        }

        element = element->next;
    }
}

void bow_parse_message(const char *expression, struct bow_config *config) {
    // check if expression is valid json, if not just make the string the expression
    // and set the type to text

    // check if first char of expression is '{'
    if (expression[0] != '{') {
        bow_log_info("Treating as string %s", expression);
        config->expression = (char *)expression;
        config->type = TEXT;
        return;
    }

    struct json_value_s *root = json_parse(expression, strlen(expression));
    if (root == NULL) {
        bow_log_error("Json not parsable, Invalid JSON: %s", expression);
        config->expression = (char *)expression;
        config->type = TEXT;
    }

    struct json_object_s *object = json_value_as_object(root);
    if (object == NULL) {
        bow_log_error("No root object found, Invalid JSON: %s", expression);
        config->expression = (char *)expression;
        config->type = TEXT;
        return;
    }

    bow_json_config_parse(object, config);
}
