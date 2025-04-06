#include "message.h"
#include "config.h"
#include "json.h"
#include "log.h"

static const char *const KEYS[] = {KEY_EXPRESSION, KEY_TYPE, KEY_SETTINGS, KEY_VERSION};

static enum pipelam_window_anchor pipelam_json_config_anchor_parse(struct json_string_s *value) {
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
        pipelam_log_error("Unknown anchor: %s", value->string);
        return CENTER;
    }
}

static void pipelam_json_config_settings_parse(struct json_object_s *object, struct pipelam_config *config) {
    char *keys[] = {"window_timeout", "anchor", "margin_left", "margin_right", "margin_top", "margin_bottom"};

    struct json_object_element_s *element = object->start;
    pipelam_log_debug("pipelam_json_config_settings_parse");
    while (element != NULL) {
        struct json_string_s *name = element->name;
        struct json_value_s *value = element->value;

        for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
            char *key = keys[i];
            if (0 == strcmp(name->string, key)) {
                pipelam_log_debug("key: %s", key);
                if (0 == strcmp(name->string, "window_timeout")) {
                    struct json_number_s *_value = json_value_as_number(value);
                    if (_value != NULL) {
                        pipelam_log_debug("window_timeout: %s", _value->number);
                        config->window_timeout = atoi(_value->number);
                        pipelam_log_debug("window_timeout: %d", config->window_timeout);
                    }
                } else if (0 == strcmp(name->string, "anchor")) {
                    struct json_string_s *_value = json_value_as_string(value);
                    pipelam_log_debug("anchor: %s", _value->string);
                    if (_value != NULL) {
                        config->anchor = pipelam_json_config_anchor_parse(_value);
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
                    pipelam_log_error("unknown key: %s", name->string);
                }
            }
        }
        element = element->next;
    }
    pipelam_log_debug("window_timeout: %d", config->window_timeout);
}

static void pipelam_json_config_parse(struct json_object_s *object, struct pipelam_config *config) {
    pipelam_log_debug("pipelam_json_config_parse");

    struct json_object_element_s *element = object->start;
    while (element != NULL) {
        struct json_string_s *name = element->name;
        struct json_value_s *value = element->value;

        for (size_t i = 0; i < sizeof(KEYS) / sizeof(KEYS[0]); i++) {

            if (0 == strcmp(name->string, KEYS[i])) {
                if (0 == strcmp(name->string, KEY_EXPRESSION)) {
                    struct json_string_s *expression = json_value_as_string(value);
                    pipelam_log_debug("expression: %s", (char *)expression->string);
                    if (expression != NULL) {
                        config->expression = (char *)expression->string;
                    } else {
                        pipelam_log_error("expression is Invalid");
                        config->expression = NULL;
                    }
                } else if (0 == strcmp(name->string, KEY_TYPE)) {
                    struct json_string_s *type = json_value_as_string(value);
                    pipelam_log_debug("type: %s", (char *)type->string);
                    if (type != NULL) {
                        if (0 == strcmp(type->string, "text")) {
                            config->type = TEXT;
                        } else if (0 == strcmp(type->string, "image")) {
                            config->type = IMAGE;
                        } else {
                            pipelam_log_error("Unknown type: %s", type->string);
                            config->type = TEXT;
                        }
                    }
                } else if (0 == strcmp(name->string, KEY_SETTINGS)) {
                    pipelam_json_config_settings_parse(json_value_as_object(value), config);
                } else if (0 == strcmp(name->string, KEY_VERSION)) {
                    struct json_string_s *version = json_value_as_string(value);
                    pipelam_log_debug("type: %s", (char *)version->string);
                } else {
                    pipelam_log_error("unknown key: %s", name->string);
                }
            }
        }

        element = element->next;
    }
}

void pipelam_parse_message(const char *expression, struct pipelam_config *config) {
    // check if first char of expression is '{'
    // I know this is dumb, but its proably the best way in this instance to check
    // parsing only occurs with an object not a list of objects, and if the the first
    // char is not '{' then we can assume its a string
    pipelam_log_debug("pipelam parse string");
    if (expression[0] != '{') {
        pipelam_log_warning("Treating as string %s", expression);
        config->expression = (char *)expression;
        config->type = TEXT;
        return;
    }

    pipelam_log_debug("pipelam parse json");
    struct json_value_s *root = json_parse(expression, strlen(expression));
    if (root == NULL) {
        pipelam_log_error("Json not parsable, Invalid JSON: %s", expression);
        config->expression = (char *)expression;
        config->type = TEXT;
        return;
    }

    pipelam_log_debug("pipelam parse json object");
    struct json_object_s *object = json_value_as_object(root);
    pipelam_log_debug("pipelam parse json object");
    if (object == NULL) {
        pipelam_log_error("No root object found, Invalid JSON: %s", expression);
        config->expression = (char *)expression;
        config->type = TEXT;
        return;
    }

    pipelam_log_debug("pipelam_json_config_parse");
    pipelam_json_config_parse(object, config);
}
