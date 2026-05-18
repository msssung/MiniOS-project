#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "osproject.h"

void init_filesystem(void);
TreeNode* create_node(const char* name, char type);
void add_child(TreeNode* parent, TreeNode* child);
TreeNode* find_child(TreeNode* parent, const char* name);
TreeNode* find_path(const char* path);
int remove_child(TreeNode* parent, const char* name);

TreeNode* get_root_directory(void);
TreeNode* get_current_directory(void);
void set_current_directory(TreeNode* node);

void print_current_path(void);

int save_filesystem(void);
int load_filesystem(void);

#endif
