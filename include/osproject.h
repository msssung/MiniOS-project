#ifndef OSPROJECT_H
#define OSPROJECT_H

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_COUNT 10
#define MAX_TOKEN_SIZE 100
#define MAX_HISTORY_COUNT 100

#define TYPE_DIR 'd'
#define TYPE_FILE 'f'

typedef struct ParsedCommand {
    char command[MAX_TOKEN_SIZE];
    char option[MAX_TOKEN_SIZE];
    char argument[MAX_INPUT_SIZE];

    int argc;

    char argv[MAX_TOKEN_COUNT][MAX_TOKEN_SIZE];

} ParsedCommand;

typedef struct TreeNode {

    char name[MAX_TOKEN_SIZE];

    char type;

    char data[MAX_INPUT_SIZE];

    int permission;

    struct TreeNode* parent;
    struct TreeNode* child;
    struct TreeNode* sibling;

} TreeNode;

typedef struct DirTree {

    TreeNode* root;
    TreeNode* current;

} DirTree;

#endif
