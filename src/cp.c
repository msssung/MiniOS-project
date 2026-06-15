#include "filesystem.h"
#include "command.h"
#include <stdio.h>
#include <string.h>

// 1. 아까 실수로 날아갔던 복사 전용 헬퍼 함수 (원상 복구)
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

// 2. 방금 전 완벽하게 수정했던 메인 cp 명령어 함수
void cmd_cp(ParsedCommand* parsed) {
    char src_name[MAX_TOKEN_SIZE] = {0,};
    char dest_name[MAX_TOKEN_SIZE] = {0,};

    if (sscanf(parsed->argument, "%s %s", src_name, dest_name) != 2) {
        printf("usage: cp <source> <destination>\n");
        return;
    }

    TreeNode* src_node = find_path(src_name);
    if (src_node == NULL) {
        printf("cp: cannot stat '%s': No such file or directory\n", src_name);
        return;
    }

    TreeNode* dest_node = find_path(dest_name);
    
    if (dest_node != NULL && dest_node->type == TYPE_DIR) {
        if (find_child(dest_node, src_node->name) != NULL) {
            printf("cp: '%s/%s' already exists\n", dest_name, src_node->name);
            return;
        }
        TreeNode* copied_tree = copy_node_recursive(src_node);
        if (copied_tree != NULL) add_child(dest_node, copied_tree);
    } else {
        char parent_path[MAX_INPUT_SIZE] = "";
        char new_file_name[MAX_TOKEN_SIZE] = "";
        char* last_slash = strrchr(dest_name, '/');
        TreeNode* target_dir = NULL;

        if (last_slash != NULL) {
            int path_len = last_slash - dest_name;
            strncpy(parent_path, dest_name, path_len);
            parent_path[path_len] = '\0';
            strcpy(new_file_name, last_slash + 1);

            target_dir = find_path(parent_path);
            if (target_dir == NULL || target_dir->type != TYPE_DIR) {
                printf("cp: cannot create regular file '%s': No such directory\n", dest_name);
                return;
            }
        } else {
            strcpy(new_file_name, dest_name);
            target_dir = get_current_directory();
        }

        if (find_child(target_dir, new_file_name) != NULL) {
            printf("cp: '%s' already exists\n", dest_name);
            return;
        }

        TreeNode* copied_tree = copy_node_recursive(src_node);
        if (copied_tree != NULL) {
            strncpy(copied_tree->name, new_file_name, MAX_TOKEN_SIZE - 1);
            copied_tree->name[MAX_TOKEN_SIZE - 1] = '\0';
            add_child(target_dir, copied_tree);
        }
    }
}
