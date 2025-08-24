#include "message.h"
#include "config.h"
#include "json.h"
#include "log.h"
#include <ctype.h>

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

    struct json_object_element_s *element = object->start;
    pipelam_log_debug("pipelam_json_config_settings_parse");
    while (element != NULL) {
        struct json_string_s *name = element->name;
        struct json_value_s *value = element->value;

        pipelam_log_debug("key: %s", name->string);
        if (0 == strcmp(name->string, "window_timeout")) {
            struct json_number_s *_value = json_value_as_number(value);
            if (_value != NULL) {
                config->window_timeout = atoi(_value->number);
            }
        } else if (0 == strcmp(name->string, "anchor")) {
            struct json_string_s *_value = json_value_as_string(value);
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
        } else if (0 == strcmp(name->string, "wob_bar_height")) {
            struct json_number_s *_value = json_value_as_number(value);
            if (_value != NULL) {
                config->wob_bar_height = atoi(_value->number);
            }
        } else if (0 == strcmp(name->string, "wob_bar_width")) {
            struct json_number_s *_value = json_value_as_number(value);
            if (_value != NULL) {
                config->wob_bar_width = atoi(_value->number);
            }
        } else if (0 == strcmp(name->string, "wob_border_color")) {
            struct json_string_s *_value = json_value_as_string(value);
            if (_value != NULL) {
                config->wob_border_color = (char *)_value->string;
            }
        } else if (0 == strcmp(name->string, "wob_background_color")) {
            struct json_string_s *_value = json_value_as_string(value);
            if (_value != NULL) {
                config->wob_background_color = (char *)_value->string;
            }
        } else if (0 == strcmp(name->string, "wob_foreground_color")) {
            struct json_string_s *_value = json_value_as_string(value);
            if (_value != NULL) {
                config->wob_foreground_color = (char *)_value->string;
            }
        } else if (0 == strcmp(name->string, "wob_overflow_color")) {
            struct json_string_s *_value = json_value_as_string(value);
            if (_value != NULL) {
                config->wob_overflow_color = (char *)_value->string;
            }
        } else if (0 == strcmp(name->string, "wob_box_color")) {
            struct json_string_s *_value = json_value_as_string(value);
            if (_value != NULL) {
                config->wob_box_color = (char *)_value->string;
            }
        } else if (0 == strcmp(name->string, "wob_box_padding")) {
            struct json_number_s *_value = json_value_as_number(value);
            if (_value != NULL) {
                config->wob_box_padding = atoi(_value->number);
            }
        } else if (0 == strcmp(name->string, "wob_border_padding")) {
            struct json_number_s *_value = json_value_as_number(value);
            if (_value != NULL) {
                config->wob_border_padding = atoi(_value->number);
            }
        } else if (0 == strcmp(name->string, "wob_border_margin")) {
            struct json_number_s *_value = json_value_as_number(value);
            if (_value != NULL) {
                config->wob_border_margin = atoi(_value->number);
            }
        } else if (0 == strcmp(name->string, "wob_background_padding")) {
            struct json_number_s *_value = json_value_as_number(value);
            if (_value != NULL) {
                config->wob_background_padding = atoi(_value->number);
            }
        } else if (0 == strcmp(name->string, "wob_foreground_padding")) {
            struct json_number_s *_value = json_value_as_number(value);
            if (_value != NULL) {
                config->wob_foreground_padding = atoi(_value->number);
            }
        } else if (0 == strcmp(name->string, "wob_foreground_overflow_padding")) {
            struct json_number_s *_value = json_value_as_number(value);
            if (_value != NULL) {
                config->wob_foreground_overflow_padding = atoi(_value->number);
            }
        } else {
            pipelam_log_debug("unknown settings key: %s", name->string);
        }

        element = element->next;
    }

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
                        } else if (0 == strcmp(type->string, "wob")) {
                            config->type = WOB;
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

static int is_valid_integer(const char *str) {
    // Skip leading whitespace
    while (*str && isspace(*str)) {
        str++;
    }
    // Check for optional sign
    if (*str == '-' || *str == '+') {
        str++;
    }
    // String must contain at least one digit
    if (!*str || !isdigit(*str)) {
        return 0;
    }
    // Check that all remaining characters are digits
    while (*str && isdigit(*str)) {
        str++;
    }
    // Skip trailing whitespace
    while (*str && isspace(*str)) {
        str++;
    }

    return !*str;
}

void pipelam_parse_message(const char *expression, struct pipelam_config *config) {
    // check if first char of expression is '{' - this could be considered a bit naive, but
    // the applicaiton is only intrested in json objects, so this is a good enough check - one thing it doesn't
    // handle is whitespace before the json object, but that could be handled by trimming the string but for now let's
    // keep it simple
    pipelam_log_debug("pipelam parse string");
    if (expression[0] != '{') {
        pipelam_log_debug("Treating as string");
        if (is_valid_integer(expression)) {
            pipelam_log_debug("Detected integer value: %s, treating as WOB", expression);
            config->expression = (char *)expression;
            config->type = WOB;
        } else {
            pipelam_log_debug("Detected string value: %s, treating as TEXT", expression);
            config->expression = (char *)expression;
            config->type = TEXT;
        }
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
