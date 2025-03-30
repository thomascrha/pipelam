
#ifndef MESSAGE_H
#define MESSAGE_H
#include "json.h"
#include "config.h"

void pipelam_json_config_parse(struct json_object_s* object, struct pipelam_config* config);
void pipelam_parse_message(const char *expression, struct pipelam_config *config);

#endif // MESSAGE_H

