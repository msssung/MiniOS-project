#include "filesystem.h"
#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

static pthread_mutex_t fs_mutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct { char path[MAX_TOKEN_SIZE]; int p_option; } MkdirArgs;

void* thread_mkdir(void *arg) {
    MkdirArgs *m_args = (MkdirArgs*)arg;
    pthread_mutex_lock(&fs_mutex);
    TreeNode *current = get_current_directory();
    TreeNode *existing = find_child(current, m_args->path);
    if (existing) {
        if (!m_args->p_option) printf("mkdir: cannot create directory '%s': File exists\n", m_args->path);
    } else {
        TreeNode *new_dir = create_node(m_args->path, TYPE_DIR);
        if (new_dir) { add_child(current, new_dir); printf("mkdir: created directory '%s' (Thread ID: %ld)\n", m_args->path, pthread_self()); }
    }
    pthread_mutex_unlock(&fs_mutex);
    free(arg);
    return NULL;
}

void cmd_mkdir(ParsedCommand* parsed) {
    if (parsed->argc <= 1) { printf("mkdir: missing operand\n"); return; }
    int p_option = 0, start_idx = 1;
    if (strcmp(parsed->option, "-p") == 0) {
        p_option = 1; start_idx = 2;
        if (parsed->argc <= 2) { printf("mkdir: missing operand after '-p'\n"); return; }
    }
    int num_dirs = parsed->argc - start_idx;
    pthread_t *threads = (pthread_t*)malloc(sizeof(pthread_t) * num_dirs);
    int t_count = 0;
    for (int i = start_idx; i < parsed->argc; i++) {
        MkdirArgs *m_args = (MkdirArgs*)malloc(sizeof(MkdirArgs));
        strncpy(m_args->path, parsed->argv[i], MAX_TOKEN_SIZE - 1);
        m_args->path[MAX_TOKEN_SIZE - 1] = '\0';
        m_args->p_option = p_option;
        if (pthread_create(&threads[t_count], NULL, thread_mkdir, (void*)m_args) != 0) { perror("mkdir: thread creation failed"); free(m_args); }
        else t_count++;
    }
    for (int i = 0; i < t_count; i++) pthread_join(threads[i], NULL);
    free(threads);
}
