#include <stdio.h>
#include <string.h>
#include "../include/osproject.h"
#include "../include/filesystem.h"
#include "../include/vstat.h"

static int calculate_total_size(TreeNode* node) {
    if (node == NULL) return 0;

    // node->size 대신 strlen(node->data)로 파일 크기 계산
    int total = (int)strlen(node->data);

    TreeNode* child = node->child;
    while (child != NULL) {
        total += calculate_total_size(child);
        child = child->sibling;
    }
    return total;
}

void cmd_vstat(ParsedCommand* parsed) {
    (void)parsed;
    TreeNode* current = get_current_directory();
    int total_size = calculate_total_size(current);

    printf("\nDisk Usage for [%s/] (Total: %d Bytes)\n", current->name, total_size);
    printf("----------------------------------------------------------\n");

    TreeNode* child = current->child;
    while (child != NULL) {
        int child_size = calculate_total_size(child);
        float percent = (total_size == 0) ? 0 : ((float)child_size / total_size) * 100;
        int bar_length = (int)(percent / 2);

        printf("%-10s |", child->name);
        for (int i = 0; i < 50; i++) printf(i < bar_length ? "#" : " ");
        printf("| %.1f%% (%d B)\n", percent, child_size);

        child = child->sibling;
    }
    printf("----------------------------------------------------------\n");
}
