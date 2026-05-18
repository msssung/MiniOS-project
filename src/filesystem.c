#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filesystem.h"

#define FILESYSTEM_SAVE_PATH "data/filesystem.dat"

static DirTree g_dir_tree;

void init_filesystem(void) {
    g_dir_tree.root = create_node("/", TYPE_DIR);
    g_dir_tree.current = g_dir_tree.root;
}

TreeNode* create_node(const char* name, char type) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));

    if (node == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    memset(node, 0, sizeof(TreeNode));

    strncpy(node->name, name, MAX_TOKEN_SIZE - 1);
    node->name[MAX_TOKEN_SIZE - 1] = '\0';

    node->type = type;
    node->permission = 755;

    node->parent = NULL;
    node->child = NULL;
    node->sibling = NULL;

    return node;
}

void add_child(TreeNode* parent, TreeNode* child) {
    if (parent == NULL || child == NULL) {
        return;
    }

    if (parent->type != TYPE_DIR) {
        printf("Parent is not a directory\n");
        return;
    }

    child->parent = parent;

    if (parent->child == NULL) {
        parent->child = child;
        return;
    }

    TreeNode* current = parent->child;

    while (current->sibling != NULL) {
        current = current->sibling;
    }

    current->sibling = child;
}

TreeNode* find_child(TreeNode* parent, const char* name) {
    if (parent == NULL || name == NULL) {
        return NULL;
    }

    TreeNode* current = parent->child;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }

        current = current->sibling;
    }

    return NULL;
}

TreeNode* find_path(const char* path) {
    if (path == NULL || strlen(path) == 0) {
        return NULL;
    }

    TreeNode* current;

    char temp[MAX_INPUT_SIZE];
    char* token;

    if (path[0] == '/') {
        current = g_dir_tree.root;
    } else {
        current = g_dir_tree.current;
    }

    strncpy(temp, path, MAX_INPUT_SIZE - 1);
    temp[MAX_INPUT_SIZE - 1] = '\0';

    token = strtok(temp, "/");

    while (token != NULL) {
        if (strcmp(token, ".") == 0) {
            token = strtok(NULL, "/");
            continue;
        }

        if (strcmp(token, "..") == 0) {
            if (current->parent != NULL) {
                current = current->parent;
            }

            token = strtok(NULL, "/");
            continue;
        }

        current = find_child(current, token);

        if (current == NULL) {
            return NULL;
        }

        token = strtok(NULL, "/");
    }

    return current;
}

int remove_child(TreeNode* parent, const char* name) {
    if (parent == NULL || name == NULL) {
        return 0;
    }

    TreeNode* current = parent->child;
    TreeNode* previous = NULL;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            if (previous == NULL) {
                parent->child = current->sibling;
            } else {
                previous->sibling = current->sibling;
            }

            free(current);
            return 1;
        }

        previous = current;
        current = current->sibling;
    }

    return 0;
}

TreeNode* get_root_directory(void) {
    return g_dir_tree.root;
}

TreeNode* get_current_directory(void) {
    return g_dir_tree.current;
}

void set_current_directory(TreeNode* node) {
    if (node != NULL && node->type == TYPE_DIR) {
        g_dir_tree.current = node;
    }
}

void print_current_path(void) {
    TreeNode* stack[100];
    int top = 0;

    TreeNode* current = g_dir_tree.current;

    while (current != NULL) {
        stack[top++] = current;
        current = current->parent;
    }

    for (int i = top - 1; i >= 0; i--) {
        if (strcmp(stack[i]->name, "/") == 0) {
            printf("/");
        } else {
            printf("%s", stack[i]->name);

            if (i != 0) {
                printf("/");
            }
        }
    }

    printf("\n");
}

static void save_node(FILE* fp, TreeNode* node, char* path) {
    if (node == NULL) {
        return;
    }

    char current_path[MAX_INPUT_SIZE];

    if (strcmp(path, "/") == 0) {
        snprintf(current_path, sizeof(current_path), "/%s", node->name);
    } else {
        snprintf(current_path, sizeof(current_path), "%s/%s", path, node->name);
    }

    fprintf(fp, "%s|%c|%d\n", current_path, node->type, node->permission);

    TreeNode* child = node->child;

    while (child != NULL) {
        save_node(fp, child, current_path);
        child = child->sibling;
    }
}

int save_filesystem(void) {
    FILE* fp = fopen(FILESYSTEM_SAVE_PATH, "w");

    if (fp == NULL) {
        printf("Failed to save filesystem\n");
        return 0;
    }

    TreeNode* child = g_dir_tree.root->child;

    fprintf(fp, "/|d|755\n");

    while (child != NULL) {
        save_node(fp, child, "/");
        child = child->sibling;
    }

    fclose(fp);
    return 1;
}

int load_filesystem(void) {
    FILE* fp = fopen(FILESYSTEM_SAVE_PATH, "r");

    if (fp == NULL) {
        init_filesystem();
        return 0;
    }

    init_filesystem();

    /*
        현재는 저장 파일 존재 여부만 확인하고 기본 root를 생성한다.
        실제 path 복원 로직은 팀원2 또는 통합 단계에서 확장 예정.
    */

    fclose(fp);
    return 1;
}
