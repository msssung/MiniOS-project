#include "../include/osproject.h"
#include "../include/history.h"
#include <stdio.h>
#include <string.h>

static char history_list[MAX_HISTORY_COUNT][MAX_INPUT_SIZE];
static int history_count = 0;

void history_add(const char* input) {
    if (history_count < MAX_HISTORY_COUNT) {
        strncpy(history_list[history_count], input, MAX_INPUT_SIZE - 1);
        history_list[history_count][MAX_INPUT_SIZE - 1] = '\0';
        history_count++;
    } else {
        for (int i = 0; i < MAX_HISTORY_COUNT - 1; i++) {
            strncpy(history_list[i], history_list[i + 1], MAX_INPUT_SIZE - 1);
        }
        strncpy(history_list[MAX_HISTORY_COUNT - 1], input, MAX_INPUT_SIZE - 1);
    }
}

void cmd_history(ParsedCommand* parsed) {
    (void)parsed;
    if (history_count == 0) {
        printf("history: no commands in history\n");
        return;
    }
    for (int i = 0; i < history_count; i++) {
        printf("%4d  %s\n", i + 1, history_list[i]);
    }
}
