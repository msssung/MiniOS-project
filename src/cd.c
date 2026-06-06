#include "filesystem.h"
#include "command.h"
#include <stdio.h>
#include <string.h>

// 현재 위치 이동 명령어 처리 함수
void cmd_cd(ParsedCommand* parsed) {
    // 인자 없으면 예외 처리 (그냥 cd만 쳤을 때)
    if (parsed->argc <= 1) {
        printf("cd: 이동할 디렉터리 이름을 입력해주세요.\n");
        return;
    }

    char *target_name = parsed->argv[1];
    TreeNode *current = get_current_directory();
    TreeNode *target = NULL;

    // 상위 디렉터리로 이동 처리 (cd ..)
    if (strcmp(target_name, "..") == 0) {
        TreeNode *parent = current->parent;
        if (parent != NULL) {
            set_current_directory(parent);
        } else {
            // 부모가 없으면 최상위 루트이므로 그대로 유지
            set_current_directory(current);
        }
        return;
    }

    // 현재 디렉터리 밑에서 입력된 이름의 자식 노드 검색
    target = find_child(current, target_name);

    if (target == NULL) {
        printf("cd: '%s'를 찾을 수 없습니다.\n", target_name);
        return;
    }

    // 찾은 노드가 디렉터리 타입인지 확인
    if (target->type != TYPE_DIR) {
        printf("cd: '%s'는 디렉터리가 아닙니다.\n", target_name);
        return;
    }

    // 해당 디렉터리로 위치 변경
    set_current_directory(target);
}