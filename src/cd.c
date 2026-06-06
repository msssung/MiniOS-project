#include "filesystem.h"
#include "command.h"
#include <stdio.h>
#include <string.h>

void cmd_cd(ParsedCommand* parsed) {
    if (parsed->argc <= 1) {
        printf("cd: missing directory argument\n");
        return;
    }
    char *target_name = parsed->argv[1];
    TreeNode *current = get_current_directory();
    if (strcmp(target_name, ".") == 0) return;
    if (strcmp(target_name, "..") == 0) {
        TreeNode *parent = current->parent;
        if (parent != NULL) set_current_directory(parent);
        return;
    }
    TreeNode *target = find_child(current, target_name);
    if (target == NULL) { printf("cd: '%s': No such file or directory\n", target_name); return; }
    if (target->type != TYPE_DIR) { printf("cd: '%s': Not a directory\n", target_name); return; }
    set_current_directory(target);
}
