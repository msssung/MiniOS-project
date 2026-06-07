#include "../include/osproject.h"
#include "../include/filesystem.h"
#include <stdio.h>
#include <string.h>

void cmd_ls(ParsedCommand* parsed) {
    // 1. 현재 머물고 있는 폴더 노드 가져오기
    TreeNode* current_dir = get_current_directory();
    
    // 2. 그 폴더 안의 첫 번째 내용물(자식 노드)로 이동
    TreeNode* current_node = current_dir->child;

    // 옵션 파악 (-a, -l)
    int opt_a = 0, opt_l = 0;
    if (strlen(parsed->option) > 0) {
        if (strstr(parsed->option, "a") != NULL) opt_a = 1;
        if (strstr(parsed->option, "l") != NULL) opt_l = 1;
    }

    // 3. 형제 노드를 타고 넘어가며 출력 반복
    while (current_node != NULL) {
        // 숨김 파일(이름이 '.'으로 시작) 처리
        if (!opt_a && current_node->name[0] == '.') {
            current_node = current_node->sibling;
            continue;
        }

        // ls -l 옵션일 때 (상세 출력)
        if (opt_l) {
            char type_char = (current_node->type == TYPE_DIR) ? 'd' : '-';
            printf("%c\tperm:%o\t%s\n", type_char, current_node->permission, current_node->name);
        } 
        // 기본 ls 일 때 (이름만 가로로 쭉 출력)
        else {
            if (current_node->type == TYPE_DIR) {
                // 폴더는 파란색으로 출력 
                printf("\033[1;34m%s\033[0m  ", current_node->name);
            } else {
                printf("%s  ", current_node->name);
            }
        }
        
        current_node = current_node->sibling; // 다음 파일 로 이동
    }
   
    if (!opt_l) {
        printf("\n"); // 마지막에 줄바꿈
    }
}
