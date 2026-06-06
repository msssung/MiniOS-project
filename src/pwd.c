#include "filesystem.h"
#include "command.h"
#include <stdio.h>

// 현재 위치의 절대 경로를 출력하는 함수
void cmd_pwd(ParsedCommand* parsed) {
    // 인자가 추가로 들어와도 pwd는 현재 경로만 보여주면 됨
    (void)parsed; 

    // filesystem.c에 구현된 공통 경로 출력 API 호출
    print_current_path();
}