#include <stdio.h>
#include <string.h>
#include "parser.h"

void parse_input(char* input, ParsedCommand* parsed) {
    char buffer[MAX_INPUT_SIZE];
    char* token;
    int index = 0;

    memset(parsed, 0, sizeof(ParsedCommand));
    strncpy(buffer, input, MAX_INPUT_SIZE - 1);
    buffer[MAX_INPUT_SIZE - 1] = '\0';

    token = strtok(buffer, " ");
    while (token != NULL && index < MAX_TOKEN_COUNT) {
        strncpy(parsed->argv[index], token, MAX_TOKEN_SIZE - 1);
        parsed->argv[index][MAX_TOKEN_SIZE - 1] = '\0';
        index++;
        token = strtok(NULL, " ");
    }
    parsed->argc = index;

    if (index > 0) {
        strncpy(parsed->command, parsed->argv[0], MAX_TOKEN_SIZE - 1);
    }

    if (index > 1) {
        int arg_start = 1;

        if (parsed->argv[1][0] == '-') {
            strncpy(parsed->option, parsed->argv[1], MAX_TOKEN_SIZE - 1);
            arg_start = 2;
        }

        if (arg_start < index) {
            char combined[MAX_INPUT_SIZE] = {0,};
            for (int i = arg_start; i < index; i++) {
                strncat(combined, parsed->argv[i],
                        MAX_INPUT_SIZE - strlen(combined) - 1);
                if (i < index - 1) {
                    strncat(combined, " ",
                            MAX_INPUT_SIZE - strlen(combined) - 1);
                }
            }
            strncpy(parsed->argument, combined, MAX_INPUT_SIZE - 1);
        }
    }
}
