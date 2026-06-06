#ifndef HISTORY_H
#define HISTORY_H

#include "osproject.h"

void history_add(const char* input);
void cmd_history(ParsedCommand* parsed);

#endif
