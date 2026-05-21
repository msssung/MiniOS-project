#include "filesystem.h"
#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// 스레드에 넘겨줄 데이터를 담는 구조체
typedef struct {
    char path[MAX_TOKEN_SIZE];
    int p_option;
} MkdirArgs;

// 각 스레드가 독립적으로 실행할 디렉터리 생성 함수
void* thread_mkdir(void* arg) {
    MkdirArgs* m_args = (MkdirArgs*)arg;

    // 동시성 제어: 트리 구조 변경 시 뮤텍스 락 설정
    pthread_mutex_lock(&fs_mutex);

    TreeNode* current = get_current_directory();
    TreeNode* existing = find_child(current, m_args->path);

    if (existing) {
        // -p 옵션이 없을 때만 이미 존재한다는 에러 메시지 출력
        if (!m_args->p_option) {
            printf("mkdir: '%s' 생성 실패: 이미 존재합니다.\n", m_args->path);
        }
    }
    else {
        // osproject.h에 정의된 TYPE_DIR 매크로('d') 사용
        TreeNode* new_dir = create_node(m_args->path, TYPE_DIR);
        if (new_dir) {
            add_child(current, new_dir);
            printf("mkdir: 디렉터리 '%s' 생성 완료 (Thread ID: %ld)\n", m_args->path, pthread_self());
        }
    }

    pthread_mutex_unlock(&fs_mutex); // 락 해제
    free(arg);
    return NULL;
}

// command.c에서 호출될 공통 API 규격 함수
void cmd_mkdir(ParsedCommand* parsed) {
    // 인자 전체 개수가 1개 이하면 폴더명을 입력하지 않은 것 
    if (parsed->argc <= 1) {
        printf("mkdir: 디렉터리 이름을 입력해주세요.\n");
        return;
    }

    int p_option = 0;
    int start_idx = 1; // 기본적으로 argv[0]이 "mkdir"이므로 폴더명은 argv[1]부터 시작

    // 팀장님 파서가 넣어준 option 필드에 "-p"가 들어가 있는지 확인
    if (strcmp(parsed->option, "-p") == 0) {
        p_option = 1;
        start_idx = 2; // "-p"가 argv[1]에 들어있으므로, 실제 폴더명은 argv[2]부터 시작

        // 만약 "mkdir -p" 까지만 치고 폴더명을 안 적었을 때 방어 코드
        if (parsed->argc <= 2) {
            printf("mkdir: -p 옵션 뒤에 디렉터리 이름을 입력해주세요.\n");
            return;
        }
    }

    // 실제 생성해야 할 폴더의 개수 계산
    int num_dirs = parsed->argc - start_idx;
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * num_dirs);
    int t_count = 0;

    // 실제 폴더명이 시작되는 인덱스(start_idx)부터 끝까지 돌면서 스레드 생성
    for (int i = start_idx; i < parsed->argc; i++) {
        MkdirArgs* m_args = (MkdirArgs*)malloc(sizeof(MkdirArgs));
        strncpy(m_args->path, parsed->argv[i], MAX_TOKEN_SIZE - 1);
        m_args->path[MAX_TOKEN_SIZE - 1] = '\0';
        m_args->p_option = p_option;

        // 폴더 하나당 스레드 하나씩 매핑하여 동시 생성 수행
        if (pthread_create(&threads[t_count], NULL, thread_mkdir, (void*)m_args) != 0) {
            perror("mkdir 스레드 생성 실패");
            free(m_args);
        }
        else {
            t_count++;
        }
    }

    // 모든 스레드가 생성을 완료할 때까지 대기 
    for (int i = 0; i < t_count; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
}