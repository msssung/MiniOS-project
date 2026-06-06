#include "../include/osproject.h"
#include "../include/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cmd_chmod(ParsedCommand* parsed) {
    char perm_str[MAX_TOKEN_SIZE];
    char target_name[MAX_TOKEN_SIZE];

    // 1. 사용자가 입력한 값에서 '권한 숫자'와 '파일 이름'을 분리해서 가져오기
    // (예: argument에 "755 file.txt"가 들어있다면 각각 쪼갬)
    if (sscanf(parsed->argument, "%s %s", perm_str, target_name) != 2) {
        printf("사용법: chmod <권한(예:755)> <파일/디렉토리명>\n");
        return;
    }

    // 2. 권한 문자를 8진수 숫자로 변환 (리눅스 권한은 8진수 기반)
    int new_permission = (int)strtol(perm_str, NULL, 8);

    // 3. 현재 디렉토리에서 목표 파일(노드) 찾기
    TreeNode* current_dir = get_current_directory();
    TreeNode* target_node = find_child(current_dir, target_name);

    if (target_node == NULL) {
        printf("chmod: '%s' 파일이나 디렉토리를 찾을 수 없습니다.\n", target_name);
        return;
    }

    // 4. 권한 변경 완료!
    target_node->permission = new_permission;
}
