#include "../include/osproject.h"
#include "../include/filesystem.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

TreeNode* copy_node_recursive(TreeNode* src_node) {
    if (src_node == NULL) return NULL;
    TreeNode* new_node = create_node(src_node->name, src_node->type);
    if (new_node == NULL) return NULL;
    new_node->permission = src_node->permission;
    strncpy(new_node->data, src_node->data, MAX_INPUT_SIZE - 1);
    TreeNode* curr_child = src_node->child;
    while (curr_child != NULL) {
        TreeNode* new_child = copy_node_recursive(curr_child);
        if (new_child != NULL) add_child(new_node, new_child);
        curr_child = curr_child->sibling;
    }
    return new_node;
}

void cmd_cp(ParsedCommand* parsed) {
    char src_name[MAX_TOKEN_SIZE] = {0,};
    char dest_name[MAX_TOKEN_SIZE] = {0,};
    if (sscanf(parsed->argument, "%s %s", src_name, dest_name) != 2) {
        printf("usage: cp <source> <destination>\n");
        return;
    }
    TreeNode* current_dir = get_current_directory();
    TreeNode* src_node = find_child(current_dir, src_name);
    if (src_node == NULL) src_node = find_path(src_name);
    if (src_node == NULL) {
        printf("cp: cannot stat '%s': No such file or directory\n", src_name);
        return;
    }
    TreeNode* dest_node = find_child(current_dir, dest_name);
    if (dest_node == NULL) dest_node = find_path(dest_name);
    if (dest_node != NULL && dest_node->type == TYPE_DIR) {
        if (find_child(dest_node, src_node->name) != NULL) {
            printf("cp: '%s/%s' already exists\n", dest_name, src_node->name);
            return;
        }
        TreeNode* copied_tree = copy_node_recursive(src_node);
        if (copied_tree != NULL) add_child(dest_node, copied_tree);
    } else {
        if (find_child(current_dir, dest_name) != NULL) {
            printf("cp: '%s' already exists\n", dest_name);
            return;
        }
        TreeNode* copied_tree = copy_node_recursive(src_node);
        if (copied_tree != NULL) {
            strncpy(copied_tree->name, dest_name, MAX_TOKEN_SIZE - 1);
            add_child(current_dir, copied_tree);
        }
    }
}
