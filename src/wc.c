#include "../include/osproject.h"
#include "../include/filesystem.h"
#include <stdio.h>
#include <string.h>

void cmd_wc(ParsedCommand* parsed) {
    if (strlen(parsed->argument) == 0) { printf("usage: wc <file>\n"); return; }
    TreeNode* current_dir = get_current_directory();
    TreeNode* file_node = find_child(current_dir, parsed->argument);
    if (file_node == NULL || file_node->type != TYPE_FILE) { printf("wc: '%s': No such file or directory\n", parsed->argument); return; }
    int bytes = strlen(file_node->data), lines = 0;
    for (int i = 0; i < bytes; i++) if (file_node->data[i] == '\n') lines++;
    if (bytes > 0 && file_node->data[bytes-1] != '\n') lines++;
    printf(" %d %d %s\n", lines, bytes, parsed->argument);
}
