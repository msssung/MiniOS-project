#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filesystem.h"

// 데이터 저장 경로 설정 (팀원들과 협의하여 필요시 "filesystem.dat" 등으로 변경 가능)
#define FILESYSTEM_SAVE_PATH "data/filesystem.dat"

// 전역 가상 파일 시스템 관리 변수
static DirTree g_dir_tree;

// ==========================================
// [내부 헬퍼] 하위 노드 연쇄 메모리 해제 (메모리 누수 완벽 차단)
// ==========================================
static void free_node_recursive(TreeNode* node) {
    if (node == NULL) return;
    
    TreeNode* child = node->child;
    while (child != NULL) {
        TreeNode* next = child->sibling;
        free_node_recursive(child);
        child = next;
    }
    free(node);
}

// ==========================================
// 1. 초기화 및 노드 생성 (엔진 기동)
// ==========================================
void init_filesystem(void) {
    g_dir_tree.root = create_node("/", TYPE_DIR);
    g_dir_tree.current = g_dir_tree.root;
}

TreeNode* create_node(const char* name, char type) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    // osproject.h의 모든 변수(data 포함)를 0(NULL)으로 안전하게 초기화
    memset(node, 0, sizeof(TreeNode));

    // MAX_TOKEN_SIZE (100) 준수: 버퍼 오버플로우 방지
    strncpy(node->name, name, MAX_TOKEN_SIZE - 1);
    node->name[MAX_TOKEN_SIZE - 1] = '\0';

    node->type = type;
    node->permission = (type == TYPE_DIR) ? 755 : 644; // 디렉터리 755, 파일 644

    node->parent = NULL;
    node->child = NULL;
    node->sibling = NULL;

    return node;
}

// ==========================================
// 2. 트리 조작 (LCRS 포인터 논리)
// ==========================================
void add_child(TreeNode* parent, TreeNode* child) {
    if (parent == NULL || child == NULL) return;

    if (parent->type != TYPE_DIR) {
        printf("Error: Parent '%s' is not a directory.\n", parent->name);
        return;
    }

    child->parent = parent;

    // 자식이 아예 없는 경우
    if (parent->child == NULL) {
        parent->child = child;
        return;
    }

    // 이미 자식이 있는 경우, 형제(sibling) 리스트의 가장 끝으로 이동하여 연결
    TreeNode* current = parent->child;
    while (current->sibling != NULL) {
        current = current->sibling;
    }
    current->sibling = child;
}

int remove_child(TreeNode* parent, const char* name) {
    if (parent == NULL || name == NULL) return 0;

    TreeNode* current = parent->child;
    TreeNode* previous = NULL;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            // 트리에서 포인터 끊어내기
            if (previous == NULL) {
                parent->child = current->sibling;
            } else {
                previous->sibling = current->sibling;
            }

            // 하위 노드까지 안전하게 메모리 반납
            free_node_recursive(current);
            return 1;
        }
        previous = current;
        current = current->sibling;
    }
    return 0;
}

// ==========================================
// 3. 경로 탐색 및 이동
// ==========================================
TreeNode* find_child(TreeNode* parent, const char* name) {
    if (parent == NULL || name == NULL) return NULL;

    TreeNode* current = parent->child;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->sibling;
    }
    return NULL;
}

TreeNode* find_path(const char* path) {
    if (path == NULL || strlen(path) == 0) return NULL;

    TreeNode* current;
    char temp[MAX_INPUT_SIZE]; // MAX_INPUT_SIZE (1024) 준수
    char* token;

    if (path[0] == '/') {
        current = g_dir_tree.root; // 절대 경로
    } else {
        current = g_dir_tree.current; // 상대 경로
    }

    strncpy(temp, path, MAX_INPUT_SIZE - 1);
    temp[MAX_INPUT_SIZE - 1] = '\0';

    token = strtok(temp, "/");
    while (token != NULL) {
        if (strcmp(token, ".") == 0) {
            token = strtok(NULL, "/");
            continue;
        }
        if (strcmp(token, "..") == 0) {
            if (current->parent != NULL) {
                current = current->parent;
            }
            token = strtok(NULL, "/");
            continue;
        }

        current = find_child(current, token);
        if (current == NULL) {
            return NULL; // 경로상에 존재하지 않는 노드 발견
        }
        token = strtok(NULL, "/");
    }

    return current;
}

// ==========================================
// 4. 상태 접근 및 출력 (Getter & Setter)
// ==========================================
TreeNode* get_root_directory(void) { return g_dir_tree.root; }
TreeNode* get_current_directory(void) { return g_dir_tree.current; }

void set_current_directory(TreeNode* node) {
    if (node != NULL && node->type == TYPE_DIR) {
        g_dir_tree.current = node;
    }
}

void print_current_path(void) {
    // MAX_HISTORY_COUNT (100) 준수
    TreeNode* stack[MAX_HISTORY_COUNT]; 
    int top = 0;

    TreeNode* current = g_dir_tree.current;
    while (current != NULL) {
        if (top >= MAX_HISTORY_COUNT) break; // 스택 오버플로우 방어
        stack[top++] = current;
        current = current->parent;
    }

    for (int i = top - 1; i >= 0; i--) {
        if (strcmp(stack[i]->name, "/") == 0) {
            printf("/");
        } else {
            printf("%s", stack[i]->name);
            if (i != 0) printf("/");
        }
    }
    printf("\n");
}

// ==========================================
// 5. 영속성 (저장 및 불러오기)
// ==========================================
static void save_node(FILE* fp, TreeNode* node, char* path) {
    if (node == NULL) return;

    char current_path[MAX_INPUT_SIZE];
    if (strcmp(path, "/") == 0) {
        snprintf(current_path, sizeof(current_path), "/%s", node->name);
    } else {
        snprintf(current_path, sizeof(current_path), "%s/%s", path, node->name);
    }

    // 포맷: 경로|타입|권한 (예: /dir1|d|755)
    fprintf(fp, "%s|%c|%d\n", current_path, node->type, node->permission);

    TreeNode* child = node->child;
    while (child != NULL) {
        save_node(fp, child, current_path);
        child = child->sibling;
    }
}

int save_filesystem(void) {
    FILE* fp = fopen(FILESYSTEM_SAVE_PATH, "w");
    if (fp == NULL) return 0;

    fprintf(fp, "/|d|755\n"); // 루트 기록
    TreeNode* child = g_dir_tree.root->child;
    while (child != NULL) {
        save_node(fp, child, "/");
        child = child->sibling;
    }

    fclose(fp);
    return 1;
}

int load_filesystem(void) {
    FILE* fp = fopen(FILESYSTEM_SAVE_PATH, "r");
    if (fp == NULL) {
        init_filesystem();
        return 0; // 저장된 파일이 없으면 초기화만 하고 종료
    }

    init_filesystem(); // 기본 루트 노드 생성
    char line[MAX_INPUT_SIZE];

    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = 0; // 개행 제거

        // 문자열 파싱 (안전한 strchr 활용)
        char* path_str = line;
        char* type_str = strchr(path_str, '|');
        if (!type_str) continue;
        *type_str = '\0'; type_str++;

        char* perm_str = strchr(type_str, '|');
        if (!perm_str) continue;
        *perm_str = '\0'; perm_str++;

        if (strcmp(path_str, "/") == 0) continue; // 루트는 건너뜀

        // 부모 경로와 이름 분리
        char parent_path[MAX_INPUT_SIZE];
        char node_name[MAX_TOKEN_SIZE];
        char* last_slash = strrchr(path_str, '/');
        
        if (last_slash == path_str) {
            strcpy(parent_path, "/");
            strncpy(node_name, last_slash + 1, MAX_TOKEN_SIZE - 1);
        } else {
            int parent_len = last_slash - path_str;
            strncpy(parent_path, path_str, parent_len);
            parent_path[parent_len] = '\0';
            strncpy(node_name, last_slash + 1, MAX_TOKEN_SIZE - 1);
        }
        node_name[MAX_TOKEN_SIZE - 1] = '\0';

        // 트리에 연결
        TreeNode* parent = find_path(parent_path);
        if (parent != NULL) {
            TreeNode* new_node = create_node(node_name, type_str[0]);
            if (new_node != NULL) {
                new_node->permission = atoi(perm_str);
                add_child(parent, new_node);
            }
        }
    }

    fclose(fp);
    return 1;
}
