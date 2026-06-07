# MiniOS

C 언어로 구현한 Linux 셸 기반 가상 파일 시스템 시뮬레이터

## 소개

MiniOS는 Linux 셸 환경에서 동작하는 가상 파일 시스템입니다. Ubuntu Linux 위에서 실행되는 C 프로그램으로, LCRS 트리 구조를 기반으로 파일 시스템을 메모리에서 관리하며 세션 간 상태를 유지합니다.

```
Ubuntu Server 22.04 LTS
└── MiniOS
    └── LCRS 트리 기반 가상 파일 시스템
```

## 실행 방법

**요구사항**: gcc, make, pthread

```bash
make
./minios
```

```bash
make clean   # 빌드 결과물 제거
make run     # 빌드 후 실행
```

## 사용 예시

```
MiniOS> mkdir a b c         # 다중 디렉토리 동시 생성 (멀티스레딩)
MiniOS> cd a
MiniOS> pwd                 # /a
MiniOS> touch file.txt
MiniOS> cat > file.txt      # 파일 쓰기 (빈 줄 입력 시 종료)
MiniOS> cat file.txt        # 파일 읽기
MiniOS> cat -n file.txt     # 줄 번호와 함께 읽기
MiniOS> wc file.txt         # 파일 통계
MiniOS> cd ..
MiniOS> chmod 755 a         # 권한 변경
MiniOS> ls -l               # 상세 목록 출력
MiniOS> cp a a_copy         # 디렉토리 복사 (깊은 복사)
MiniOS> vstat               # 디스크 사용량 시각화
MiniOS> rmdir -p a          # 연쇄 디렉토리 삭제
MiniOS> exit                # 저장 후 종료
```

## 지원 명령어

| 명령어 | 설명 |
|--------|------|
| `ls` | 디렉토리 목록 출력 (`-a`, `-l`, `-al` 옵션) |
| `cd` | 디렉토리 이동 (`.`, `..` 지원) |
| `mkdir` | 디렉토리 생성 (`-p` 옵션, **멀티스레딩**) |
| `cat` | 파일 읽기/쓰기 (`>` 쓰기 모드, `-n` 줄 번호) |
| `pwd` | 현재 경로 출력 |
| `chmod` | 파일 권한 변경 (8진수) |
| `clear` | 터미널 화면 초기화 |
| `cp` | 파일/디렉토리 복사 (재귀적 깊은 복사) |
| `rmdir` | 디렉토리 삭제 (`-p` 연쇄 삭제) |
| `touch` | 파일 생성 / 타임스탬프 갱신 |
| `wc` | 줄 수 및 바이트 수 출력 |
| `vstat` | 막대 그래프 기반 디스크 사용량 시각화 |

## 주요 설계

- **LCRS 트리**: parent/child/sibling 포인터를 가진 양방향 트리로 효율적인 탐색 구현
- **멀티스레딩**: `mkdir`에서 디렉토리마다 독립적인 pthread 생성, mutex로 동기화
- **영속성**: 종료 시 `data/filesystem.dat`에 직렬화 저장, 재시작 시 자동 복원
- **모듈화**: 명령어별 독립 `.c` 파일로 분리하여 확장성 확보

## 프로젝트 구조

```
MiniOS/
├── include/
│   ├── osproject.h     # 전역 상수, TreeNode, ParsedCommand 구조체
│   ├── filesystem.h    # 파일 시스템 API 선언
│   ├── command.h       # 명령어 디스패처 선언
│   ├── parser.h        # 파서 선언
│   └── vstat.h         # vstat 선언
├── src/
│   ├── main.c          # 메인 루프
│   ├── parser.c        # 입력 파싱
│   ├── command.c       # 명령어 디스패치
│   ├── filesystem.c    # 파일 시스템 코어 엔진
│   ├── ls.c / cd.c / mkdir.c / cat.c / pwd.c
│   ├── chmod.c / clear.c / cp.c / rmdir.c
│   └── touch.c / wc.c / vstat.c
├── data/
│   └── filesystem.dat  # 파일 시스템 상태 저장 파일
└── Makefile
```

## 기술 스택

- **언어**: C (C99)
- **라이브러리**: pthread (POSIX Threads)
- **플랫폼**: Ubuntu Server 22.04 LTS
- **빌드**: GNU Make, GCC
