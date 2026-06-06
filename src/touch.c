#include "filesystem.h"
#include "command.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>

// mkdir이랑 똑같이 동시성 제어용 임시 로컬 락 선언
static pthread_mutex_t local_touch_mutex = PTHREAD_MUTEX_INITIALIZER;

// 빈 파일 생성 명령어 처리 함수
void cmd_touch(ParsedCommand* parsed) {
    // 파일 이름 안 적었으면 예외 처리
    if (parsed->argc <= 1) {
        printf("touch: 파일 이름을 입력해주세요.\n");
        return;
    }

    char *file_name = parsed->argv[1];

    // 트리 구조 건드릴 거니까 락 걸기
    pthread_mutex_lock(&local_touch_mutex);

    TreeNode *current = get_current_directory();
    TreeNode *existing = find_child(current, file_name);

    if (existing) {
        // 이미 같은 이름의 노드가 존재할 때
        if (existing->type == TYPE_FILE) {
            printf("touch: '%s' 파일의 시간 정보를 갱신했습니다.\n", file_name);
        } else {
            printf("touch: '%s'는 이미 존재하는 디렉터리 이름입니다.\n", file_name);
        }
    } else {
        // osproject.h에 선언된 TYPE_FILE 매크로('f') 사용해서 빈 파일 생성
        TreeNode *new_file = create_node(file_name, TYPE_FILE);
        if (new_file) {
            add_child(current, new_file);
            printf("touch: 빈 파일 '%s'를 생성했습니다.\n", file_name);
        }
    }

    pthread_mutex_unlock(&local_touch_mutex); // 락 해제
}