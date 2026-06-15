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
    TreeNode *target = find_path(target_name);
    
    if (target == NULL) { 
        printf("cd: '%s': No such file or directory\n", target_name); 
        return; 
    }
    if (target->type != TYPE_DIR) { 
        printf("cd: '%s': Not a directory\n", target_name); 
        return; 
    }
    
    set_current_directory(target);
}
