#include "../include/osproject.h"
#include "../include/filesystem.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void cmd_cat(ParsedCommand* parsed) {
    TreeNode* current_dir = get_current_directory();
    int opt_n = 0;
    int write_mode = 0;
    char target_name[MAX_TOKEN_SIZE] = {0,};

    // 1. -n 옵션이 포함되어 있는지 확인
    if (strstr(parsed->option, "n") != NULL) {
        opt_n = 1;
    }

    // 2. argument 분석 ('> file.txt' 형태의 쓰기 모드인지, 그냥 'file.txt' 읽기 모드인지 분리)
    char* arg_ptr = parsed->argument;
    while (*arg_ptr == ' ') arg_ptr++; // 앞쪽 공백 제거

    if (arg_ptr[0] == '>') {
        write_mode = 1;
        arg_ptr++; // '>' 기호 건너뛰기
        while (*arg_ptr == ' ') arg_ptr++; // 파일명 앞 공백 제거
        strncpy(target_name, arg_ptr, MAX_TOKEN_SIZE - 1);
    } else {
        strncpy(target_name, arg_ptr, MAX_TOKEN_SIZE - 1);
    }

    // 파일명이 입력되지 않은 경우 예외 처리
    if (strlen(target_name) == 0) {
        printf("사용법:\n  cat <파일명>\n  cat -n <파일명>\n  cat > <파일명>\n");
        return;
    }

    // 3. 쓰기 모드 처리 (cat > file.txt)
    if (write_mode) {
        TreeNode* file_node = find_child(current_dir, target_name);
        
        // 파일이 없으면 새로 생성
        if (file_node == NULL) {
            file_node = create_node(target_name, TYPE_FILE);
            if (file_node == NULL) {
                printf("cat: 파일을 생성할 수 없습니다.\n");
                return;
            }
            add_child(current_dir, file_node); // 현재 디렉토리에 새 파일 연결
        } else if (file_node->type != TYPE_FILE) {
            printf("cat: '%s'은(는) 디렉토리입니다.\n", target_name);
            return;
        }

        // 기존 데이터 초기화 후 새로 입력 받기
        memset(file_node->data, 0, sizeof(file_node->data));
        char buffer[MAX_INPUT_SIZE] = {0,};
        printf("파일 내용을 입력하세요. (종료하려면 빈 줄에서 엔터(Enter)를 누르세요):\n");

        while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // 아무것도 안 치고 엔터만 누르면 입력 종료
            if (strcmp(buffer, "\n") == 0) {
                break;
            }
            // 버퍼 오버플로우 방지 (승우님이 강조한 규칙 준수)
            if (strlen(file_node->data) + strlen(buffer) < MAX_INPUT_SIZE) {
                strcat(file_node->data, buffer);
            } else {
                printf("cat: 최대 파일 크기 한도를 초과하여 입력을 중단합니다.\n");
                break;
            }
        }
        printf("파일 저장이 완료되었습니다.\n");

    } 
    // 4. 읽기 모드 처리 (cat file.txt / cat -n file.txt)
    else {
        TreeNode* file_node = find_child(current_dir, target_name);
        if (file_node == NULL) {
            printf("cat: '%s' 파일이 존재하지 않습니다.\n", target_name);
            return;
        }
        if (file_node->type != TYPE_FILE) {
            printf("cat: '%s'은(는) 디렉토리입니다.\n", target_name);
            return;
        }

        // -n 옵션: 줄 번호와 함께 출력
        if (opt_n) {
            int line_num = 1;
            char data_copy[MAX_INPUT_SIZE];
            strncpy(data_copy, file_node->data, MAX_INPUT_SIZE - 1);
            data_copy[MAX_INPUT_SIZE - 1] = '\0';

            char* line = strtok(data_copy, "\n");
            while (line != NULL) {
                printf("%6d\t%s\n", line_num++, line);
                line = strtok(NULL, "\n");
            }
        } 
        // 기본 출력
        else {
            printf("%s", file_node->data);
            if (strlen(file_node->data) > 0 && file_node->data[strlen(file_node->data) - 1] != '\n') {
                printf("\n");
            }
        }
    }
}
