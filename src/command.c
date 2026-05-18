#include <stdio.h>
#include <string.h>

#include "command.h"

void dispatch_command(ParsedCommand* parsed) {
    printf("Command received: %s\n", parsed->command);
}
