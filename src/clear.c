#include "../include/osproject.h"
#include <stdio.h>

void cmd_clear(ParsedCommand* parsed) {
    // 터미널 화면을 깨끗하게 지우는 ANSI Escape Code
    printf("\033[H\033[J");
}
