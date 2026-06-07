#include "filesystem.h"
#include "command.h"
#include <stdio.h>


void cmd_pwd(ParsedCommand* parsed) {
    (void)parsed; 
    print_current_path();
}