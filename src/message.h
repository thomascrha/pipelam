
#ifndef MESSAGE_H
#define MESSAGE_H
#include "config.h"

#define KEY_EXPRESSION "expression"
#define KEY_TYPE "type"
#define KEY_SETTINGS "settings"
#define KEY_VERSION "version"

void pipelam_parse_message(const char *expression, struct pipelam_config *config);

#endif // MESSAGE_H
