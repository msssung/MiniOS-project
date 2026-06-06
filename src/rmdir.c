#include "filesystem.h"
#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// mkdir이랑 맞춰서 임시로 쓸 로컬 뮤텍스 락 선언
static pthread_mutex_t local_rmdir_mutex = PTHREAD_MUTEX_INITIALIZER;

// command.c에서 연결해줄 rmdir 메인 함수
void cmd_rmdir(ParsedCommand* parsed) {
    // 인자가 없으면 예외 처리
    if (parsed->argc <= 1) {
        printf("rmdir: 디렉터리 이름을 입력해주세요.\n");
        return;
    }

    int p_option = 0;
    int target_idx = 1; // 기본적으로 rmdir 뒤에 오는 argv[1]이 타겟 폴더명

    
    if (strcmp(parsed->option, "-p") == 0) {
        p_option = 1;
        target_idx = 2; // -p가 argv[1]에 있으니까 실제 폴더명은 argv[2]가 됨
        
        if (parsed->argc <= 2) {
            printf("rmdir: -p 옵션 뒤에 디렉터리 이름을 입력해주세요.\n");
            return;
        }
    }

    char *target_name = parsed->argv[target_idx];

    pthread_mutex_lock(&local_rmdir_mutex);

    TreeNode *current = get_current_directory();
    TreeNode *target = find_child(current, target_name);
    
    // 지우려는 폴더가 진짜 존재하는지 확인
    if (!target) {
        printf("rmdir: '%s'를 찾을 수 없습니다.\n", target_name);
        pthread_mutex_unlock(&local_rmdir_mutex);
        return;
    }

    // 파일 시스템 규칙: 디렉터리 타입인지 검사
    if (target->type != TYPE_DIR) {
        printf("rmdir: '%s'는 디렉터리가 아닙니다.\n", target_name);
        pthread_mutex_unlock(&local_rmdir_mutex);
        return;
    }

    // 파일 시스템 규칙: 디렉터리 내부에 자식(파일이나 하위 폴더)이 있으면 rmdir 불가
    if (target->child != NULL) {
        printf("rmdir: '%s' 디렉터리가 비어있지 않습니다.\n", target_name);
        pthread_mutex_unlock(&local_rmdir_mutex);
        return;
    }

    // 상향식 연쇄 삭제 알고리즘 시작
    TreeNode *curr = target;
    TreeNode *root_dir = get_root_directory();
    
    // 루트 노드가 아니고, 자식이 비어있는 디렉터리일 때만 계속 위로 올라가면서 삭제
    while (curr != root_dir && curr->child == NULL) {
        TreeNode *parent = curr->parent;
        char deleted_name[MAX_TOKEN_SIZE];
        strcpy(deleted_name, curr->name);
        
        //  remove_child API 호출 (트리에서 포인터 끊고 메모리 해제까지 해줌)
        int success = remove_child(parent, deleted_name);
        
        if (success) {
            printf("rmdir: 디렉터리 '%s' 삭제 완료\n", deleted_name);
        } else {
            break; 
        }
        
        // -p 옵션이 안 들어왔으면 연쇄 삭제 안 하고 한 번만 지운 뒤 루프 탈출
        if (!p_option) {
            break;
        }
        
        // 부모 디렉터리로 한 단계 거슬러 올라감 (상향식 탐색)
        curr = parent; 
    }

    pthread_mutex_unlock(&local_rmdir_mutex); // 작업 끝났으니 락 해제
}