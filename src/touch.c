#include "filesystem.h"
#include "command.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>

static pthread_mutex_t local_touch_mutex = PTHREAD_MUTEX_INITIALIZER;

void cmd_touch(ParsedCommand* parsed) {
    if (parsed->argc <= 1) { printf("touch: missing file operand\n"); return; }
    char *file_name = parsed->argv[1];
    pthread_mutex_lock(&local_touch_mutex);
    TreeNode *current = get_current_directory();
    TreeNode *existing = find_child(current, file_name);
    if (existing) {
        if (existing->type == TYPE_FILE) printf("touch: '%s': timestamp updated\n", file_name);
        else printf("touch: '%s': Is a directory\n", file_name);
    } else {
        TreeNode *new_file = create_node(file_name, TYPE_FILE);
        if (new_file) { add_child(current, new_file); printf("touch: created '%s'\n", file_name); }
    }
    pthread_mutex_unlock(&local_touch_mutex);
}
