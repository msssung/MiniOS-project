#include "filesystem.h"
#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


static pthread_mutex_t fs_mutex = PTHREAD_MUTEX_INITIALIZER;

// 스레드 인자용 구조체
typedef struct {
    char path[MAX_TOKEN_SIZE];
    int p_option;
} MkdirArgs;

// 스레드가 각각 돌릴 폴더 생성 함수
void* thread_mkdir(void *arg) {
    MkdirArgs *m_args = (MkdirArgs*)arg;
    
    // 트리 메모리 꼬임 방지용 뮤텍스 락
    pthread_mutex_lock(&fs_mutex);
    
    TreeNode *current = get_current_directory();
    TreeNode *existing = find_child(current, m_args->path);
    
    if (existing) {
        // -p 옵션 꺼져있을 때만 중복 에러 출력
        if (!m_args->p_option) {
            printf("mkdir: '%s' 생성 실패: 이미 존재합니다.\n", m_args->path);
        }
    } else {
        // 실제 노드 생성 및 추가
        TreeNode *new_dir = create_node(m_args->path, TYPE_DIR);
        if (new_dir) {
            add_child(current, new_dir);
            // 시연 때 스레드 여러개 도는 거 보여주려고 ID 같이 찍음
            printf("mkdir: 디렉터리 '%s' 생성 완료 (Thread ID: %ld)\n", m_args->path, pthread_self());
        }
    }
    
    pthread_mutex_unlock(&fs_mutex); // 락 해제
    free(arg); // malloc 해제
    return NULL;
}

// 메인 쉘 매핑 함수
void cmd_mkdir(ParsedCommand* parsed) {
    // 예외처리: 인자 없을 때
    if (parsed->argc <= 1) {
        printf("mkdir: 디렉터리 이름을 입력해주세요.\n");
        return;
    }

    int p_option = 0;
    int start_idx = 1; // argv[0]은 명령어 이름이라 1번 인덱스부터 시작

    // -p 옵션 파싱 처리
    if (strcmp(parsed->option, "-p") == 0) {
        p_option = 1;
        start_idx = 2; // -p가 argv[1]이라 폴더명은 2번부터
        
        if (parsed->argc <= 2) {
            printf("mkdir: -p 옵션 뒤에 디렉터리 이름을 입력해주세요.\n");
            return;
        }
    }

    // 폴더 개수만큼 스레드 할당
    int num_dirs = parsed->argc - start_idx;
    pthread_t *threads = (pthread_t*)malloc(sizeof(pthread_t) * num_dirs);
    int t_count = 0;

    // 인자 돌면서 스레드 생성
    for (int i = start_idx; i < parsed->argc; i++) {
        MkdirArgs *m_args = (MkdirArgs*)malloc(sizeof(MkdirArgs));
        strncpy(m_args->path, parsed->argv[i], MAX_TOKEN_SIZE - 1);
        m_args->path[MAX_TOKEN_SIZE - 1] = '\0';
        m_args->p_option = p_option;

        if (pthread_create(&threads[t_count], NULL, thread_mkdir, (void*)m_args) != 0) {
            perror("mkdir 스레드 생성 실패");
            free(m_args);
        } else {
            t_count++;
        }
    }

    // 일꾼 스레드 다 끝날 때까지 대기
    for (int i = 0; i < t_count; i++) {
        pthread_join(threads[i], NULL);
    }
    
    free(threads);
}