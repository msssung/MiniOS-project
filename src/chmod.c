#include "../include/osproject.h"
#include "../include/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cmd_chmod(ParsedCommand* parsed) {
    char perm_str[MAX_TOKEN_SIZE], target_name[MAX_TOKEN_SIZE];
    if (sscanf(parsed->argument, "%s %s", perm_str, target_name) != 2) {
        printf("usage: chmod <mode> <file>\n");
        return;
    }
    int new_permission = (int)strtol(perm_str, NULL, 8);
    TreeNode* current_dir = get_current_directory();
    TreeNode* target_node = find_child(current_dir, target_name);
    if (target_node == NULL) {
        printf("chmod: cannot access '%s': No such file or directory\n", target_name);
        return;
    }
    target_node->permission = new_permission;
    printf("chmod: '%s' permissions changed to %s\n", target_name, perm_str);
}
