#ifndef CLI_H
#define CLI_H

#include "config.h"

void pipelam_process_command_line_args(int argc, char *argv[], struct pipelam_config *config);
void pipelam_help(void);

#endif // CLI_H

