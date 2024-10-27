
#ifndef MESSAGE_H
#define MESSAGE_H
#include "json.h"
#include "config.h"

void bow_json_config_parse(struct json_object_s* object, struct bow_config* config);
void bow_parse_message(const char *expression, struct bow_config *config);

#endif // MESSAGE_H

