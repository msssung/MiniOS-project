#include "../include/osproject.h"
#include <stdio.h>

void cmd_clear(ParsedCommand* parsed) {
    (void)parsed;
    printf("\033[H\033[J");
}
