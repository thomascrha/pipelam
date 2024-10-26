#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "json.h"
#include "config.h"

struct bow_json_config_settings {
    int window_timeout;
    char* volume_expression;
    enum bow_window_anchor anchor;
    int margin_left;
    int margin_right;
    int margin_top;
    int margin_bottom;
};

struct bow_json_config {
    char* type;
    char* expression;
    struct bow_json_config_settings settings;
    char* description;
};

static struct bow_json_config_settings* bow_json_config_settings_parse(struct json_object_s* object) {
	struct bow_json_config_settings settings = {0};

	char* keys[] = {"window_timeout", "volume_expression", "anchor", "margin_left", "margin_right", "margin_top", "margin_bottom"};

	struct json_object_element_s* element = object->start;
	while (element != NULL) {
		struct json_string_s* name = element->name;
		struct json_value_s* value = element->value;
		// struct json_object_s* value_object = json_value_as_object(value); // Use a different variable
		struct json_string_s* value_string = json_value_as_string(value);

		for (size_t i = 0; i < sizeof(keys)/sizeof(keys[0]); i++) {
			char* key = keys[i];

			if (0 == strcmp(name->string, key)) {

				if (0 == strcmp(name->string, "window_timeout")) {
					settings.window_timeout = atoi(value_string->string);

				} else if (0 == strcmp(name->string, "volume_expression")) {
					settings.volume_expression = (char*)value_string->string;

				} else if (0 == strcmp(name->string, "anchor")) {
					settings.anchor = (enum bow_window_anchor)atoi(value_string->string);

				} else if (0 == strcmp(name->string, "margin_left")) {
					settings.margin_left = atoi(value_string->string);

				} else if (0 == strcmp(name->string, "margin_right")) {
					settings.margin_right = atoi(value_string->string);

				} else if (0 == strcmp(name->string, "margin_top")) {
					settings.margin_top = atoi(value_string->string);

				} else if (0 == strcmp(name->string, "margin_bottom")) {
					settings.margin_bottom = atoi(value_string->string);

				} else {
					printf("unknown key: %s\n", name->string);
				}
			}
		}

		element = element->next;
	}

	return settings;
}

static int bow_test_json(const char* json) {
    struct json_value_s* root = json_parse(json, strlen(json));

    struct json_object_s* object = json_value_as_object(root);
    if (object == NULL) {
        printf("json_value_as_object failed\n");
        return EXIT_FAILURE;
    }

    char* exprssion = NULL;
    char* type = NULL;
    struct json_object_s* settings = NULL;
    char* description = NULL;

    char* keys[] = {"type", "expression"};
    char* optional_keys[] = {"settings", "description"};

    struct json_object_element_s* element = object->start;
    while (element != NULL) {
        struct json_string_s* name = element->name;
        struct json_value_s* value = element->value;
        struct json_object_s* value_object = json_value_as_object(value); // Use a different variable
        struct json_string_s* value_string = json_value_as_string(value);

        for (size_t i = 0; i < sizeof(keys)/sizeof(keys[0]) + sizeof(optional_keys)/sizeof(optional_keys[0]); i++) {
            char* key = i < sizeof(keys)/sizeof(keys[0]) ? keys[i] : optional_keys[i - sizeof(keys)/sizeof(keys[0])];

            if (0 == strcmp(name->string, key)) {

                if (0 == strcmp(name->string, "expression")) {
                    exprssion = (char*)value_string->string;

				} else if (0 == strcmp(name->string, "type")) {
                    type = (char*)value_string->string;

				} else if (0 == strcmp(name->string, "settings")) {
                    settings = bow_json_config_settings_parse(value_object);
                    // settings = value_object; // Use the correct variable

				} else if (0 == strcmp(name->string, "description")) {
					description = (char*)value_string->string;

				} else {
                    printf("unknown key: %s\n", name->string);
                }
            }
        }

        element = element->next;
    }

    if (exprssion == NULL || type == NULL) {
        printf("missing required fields\n");
        return EXIT_FAILURE;
    }

    printf("type: %s\n", type);
    printf("expression: %s\n", exprssion);

    if (settings != NULL) {
        printf("settings: %s\n", settings->window_timeout);
    }

    if (description != NULL) {
        printf("description: %s\n", description);
    }

    free(root);
    return 0;
}

int main(void) {
    const char *json = "{\"type\" : \"text\", \"expression\" : \"/some/path/to/a/file\", \"settings\" : {\"window_timeout\": 600}, \"description\" : \"description_value\"}";

    return bow_test_json(json);
}
