#include "filesystem.h"
#include "command.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>

static pthread_mutex_t local_rmdir_mutex = PTHREAD_MUTEX_INITIALIZER;

void cmd_rmdir(ParsedCommand* parsed) {
    if (parsed->argc <= 1) { printf("rmdir: missing operand\n"); return; }
    int p_option = 0, target_idx = 1;
    if (strcmp(parsed->option, "-p") == 0) {
        p_option = 1; target_idx = 2;
        if (parsed->argc <= 2) { printf("rmdir: missing operand after '-p'\n"); return; }
    }
    char *target_name = parsed->argv[target_idx];
    pthread_mutex_lock(&local_rmdir_mutex);
    TreeNode *current = get_current_directory();
    TreeNode *target = find_child(current, target_name);
    if (!target) { printf("rmdir: failed to remove '%s': No such file or directory\n", target_name); pthread_mutex_unlock(&local_rmdir_mutex); return; }
    if (target->type != TYPE_DIR) { printf("rmdir: failed to remove '%s': Not a directory\n", target_name); pthread_mutex_unlock(&local_rmdir_mutex); return; }
    if (target->child != NULL) { printf("rmdir: failed to remove '%s': Directory not empty\n", target_name); pthread_mutex_unlock(&local_rmdir_mutex); return; }
    TreeNode *curr = target, *root_dir = get_root_directory();
    while (curr != root_dir && curr->child == NULL) {
        TreeNode *parent = curr->parent;
        char deleted_name[MAX_TOKEN_SIZE];
        strcpy(deleted_name, curr->name);
        if (remove_child(parent, deleted_name)) printf("rmdir: removed directory '%s'\n", deleted_name);
        else break;
        if (!p_option) break;
        curr = parent;
    }
    pthread_mutex_unlock(&local_rmdir_mutex);
}
