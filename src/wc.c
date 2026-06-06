#include "../include/osproject.h"
#include "../include/filesystem.h"
#include <stdio.h>
#include <string.h>

void cmd_wc(ParsedCommand* parsed) {
    // 인자(파일명)가 없는 경우
    if (strlen(parsed->argument) == 0) {
        printf("사용법: wc <파일명>\n");
        return;
    }

    TreeNode* current_dir = get_current_directory();
    TreeNode* file_node = find_child(current_dir, parsed->argument);

    // 파일이 없거나 디렉토리인 경우
    if (file_node == NULL || file_node->type != TYPE_FILE) {
        printf("wc: '%s' 파일을 찾을 수 없습니다.\n", parsed->argument);
        return;
    }

    // 글자 수(Byte) 및 줄 수 계산
    int bytes = strlen(file_node->data);
    int lines = 0;
    
    for (int i = 0; i < bytes; i++) {
        if (file_node->data[i] == '\n') lines++;
    }
    // 마지막 줄에 엔터가 없어도 1줄로 침
    if (bytes > 0 && file_node->data[bytes-1] != '\n') lines++; 

    printf(" %d줄, %d바이트 %s\n", lines, bytes, parsed->argument);
}
