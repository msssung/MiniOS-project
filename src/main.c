#include <stdio.h>
#include <string.h>

#include "osproject.h"
#include "parser.h"
#include "command.h"
#include "filesystem.h"

int main(void) {
    char input[MAX_INPUT_SIZE];
    ParsedCommand parsed;

    load_filesystem();

    printf("MiniOS Started\n");

    while (1) {
        printf("MiniOS> ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0) {
            continue;
        }

        if (strcmp(input, "exit") == 0) {
            printf("MiniOS Shutdown\n");
            save_filesystem();
            break;
        }

        parse_input(input, &parsed);
        dispatch_command(&parsed);
    }

    return 0;
}
