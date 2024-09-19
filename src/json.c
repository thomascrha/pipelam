#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "json.h"

int main(void) {
    const char json[] = "{\"type\" : \"text\", \"expresion\" : \"/some/path/to/a/file\", \"config\" : \"config_value\", \"description\" : \"description_value\"}";
    struct json_value_s* root = json_parse(json, strlen(json));

    struct json_object_s* object = json_value_as_object(root);
	if (object == NULL) {
		printf("json_value_as_object failed\n");
		return EXIT_FAILURE;
	}

    char* exprssion = NULL;
    char* type = NULL;
    char* config = NULL;

    char* keys[] = {"type", "expression"};
    char* optional_keys[] = {"config"};

    struct json_object_element_s* element = object->start;
    while (element != NULL) {
        struct json_string_s* name = element->name;
        struct json_value_s* value = element->value;
        struct json_string_s* string = json_value_as_string(value);

        for (size_t i = 0; i < sizeof(keys)/sizeof(keys[0]) + sizeof(optional_keys)/sizeof(optional_keys[0]); i++) {
            char* key = i < sizeof(keys)/sizeof(keys[0]) ? keys[i] : optional_keys[i - sizeof(keys)/sizeof(keys[0])];
            if (0 == strcmp(name->string, key)) {
                if (0 == strcmp(name->string, "expression")) {
                    exprssion = (char*)string->string;
                } else if (0 == strcmp(name->string, "type")) {
                    type = (char*)string->string;
                } else if (0 == strcmp(name->string, "config")) {
                    config = (char*)string->string;
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

    if (config != NULL) {
        printf("config: %s\n", config);
    }

    free(root);
    return 0;
}
