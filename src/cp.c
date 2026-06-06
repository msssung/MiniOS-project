#include "../include/osproject.h"
#include "../include/filesystem.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// [헬퍼 함수] 원본 노드와 그 하위 자식 트리를 통째로 복사하는 재귀 함수
TreeNode* copy_node_recursive(TreeNode* src_node) {
    if (src_node == NULL) return NULL;

    // 1. 새 노드 생성 (이름과 타입 복사)
    TreeNode* new_node = create_node(src_node->name, src_node->type);
    if (new_node == NULL) return NULL;

    // 2. 권한 및 데이터 복사
    new_node->permission = src_node->permission;
    strncpy(new_node->data, src_node->data, MAX_INPUT_SIZE - 1);

    // 3. 자식 노드들을 순회하며 연쇄적으로 복사 (LCRS 트리 구조 반영)
    TreeNode* curr_child = src_node->child;
    while (curr_child != NULL) {
        TreeNode* new_child = copy_node_recursive(curr_child);
        if (new_child != NULL) {
            add_child(new_node, new_child); // 새 부모 노드에 복사된 자식 연결
        }
        curr_child = curr_child->sibling; // 다음 형제 자식으로 이동
    }

    return new_node;
}

void cmd_cp(ParsedCommand* parsed) {
    char src_name[MAX_TOKEN_SIZE] = {0,};
    char dest_name[MAX_TOKEN_SIZE] = {0,};

    // 1. 사용자가 입력한 인자에서 원본(src)과 대상(dest) 이름 추출
    if (sscanf(parsed->argument, "%s %s", src_name, dest_name) != 2) {
        printf("사용법: cp <원본 파일/디렉토리> <대상 파일/디렉토리>\n");
        return;
    }

    TreeNode* current_dir = get_current_directory();

    // 2. 원본 노드 찾기 (현재 디렉토리에서 먼저 찾고, 없으면 전체 경로 검색)
    TreeNode* src_node = find_child(current_dir, src_name);
    if (src_node == NULL) {
        src_node = find_path(src_name);
    }

    if (src_node == NULL) {
        printf("cp: '%s'을(는) 찾을 수 없습니다.\n", src_name);
        return;
    }

    // 3. 대상 노드 찾기
    TreeNode* dest_node = find_child(current_dir, dest_name);
    if (dest_node == NULL) {
        dest_node = find_path(dest_name);
    }

    // 상황 A: 대상이 이미 존재하는 '디렉토리'인 경우 (그 디렉토리 내부로 원본을 복사)
    if (dest_node != NULL && dest_node->type == TYPE_DIR) {
        // 이미 대상 디렉토리 안에 같은 이름의 노드가 있는지 확인
        if (find_child(dest_node, src_node->name) != NULL) {
            printf("cp: '%s/%s'가 이미 존재합니다.\n", dest_name, src_node->name);
            return;
        }

        TreeNode* copied_tree = copy_node_recursive(src_node);
        if (copied_tree != NULL) {
            add_child(dest_node, copied_tree);
        }
    }
    // 상황 B: 대상이 존재하지 않거나 파일인 경우 (새로운 이름으로 복사본 생성)
    else {
        // 현재 디렉토리에 같은 이름의 파일/폴더가 이미 있는지 확인
        if (find_child(current_dir, dest_name) != NULL) {
            printf("cp: '%s'가 이미 존재합니다.\n", dest_name);
            return;
        }

        TreeNode* copied_tree = copy_node_recursive(src_node);
        if (copied_tree != NULL) {
            // 복사된 최상위 노드의 이름을 대상 이름(dest_name)으로 변경
            strncpy(copied_tree->name, dest_name, MAX_TOKEN_SIZE - 1);
            add_child(current_dir, copied_tree);
        }
    }
}
