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

    if (strstr(parsed->option, "n") != NULL) {
        opt_n = 1;
    }

    char* arg_ptr = parsed->argument;
    while (*arg_ptr == ' ') arg_ptr++;

    if (arg_ptr[0] == '>') {
        write_mode = 1;
        arg_ptr++;
        while (*arg_ptr == ' ') arg_ptr++;
        strncpy(target_name, arg_ptr, MAX_TOKEN_SIZE - 1);
    } else {
        strncpy(target_name, arg_ptr, MAX_TOKEN_SIZE - 1);
    }

    if (strlen(target_name) == 0) {
        printf("usage:\n  cat <file>\n  cat -n <file>\n  cat > <file>\n");
        return;
    }

    if (write_mode) {
        TreeNode* file_node = find_child(current_dir, target_name);
        if (file_node == NULL) {
            file_node = create_node(target_name, TYPE_FILE);
            if (file_node == NULL) {
                printf("cat: cannot create file\n");
                return;
            }
            add_child(current_dir, file_node);
        } else if (file_node->type != TYPE_FILE) {
            printf("cat: '%s': Is a directory\n", target_name);
            return;
        }
        memset(file_node->data, 0, sizeof(file_node->data));
        char buffer[MAX_INPUT_SIZE] = {0,};
        printf("Enter file content (press Enter on empty line to finish):\n");
        while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (strcmp(buffer, "\n") == 0) break;
            if (strlen(file_node->data) + strlen(buffer) < MAX_INPUT_SIZE) {
                strcat(file_node->data, buffer);
            } else {
                printf("cat: input size limit exceeded\n");
                break;
            }
        }
        printf("cat: file saved\n");
    } else {
        TreeNode* file_node = find_child(current_dir, target_name);
        if (file_node == NULL) {
            printf("cat: '%s': No such file or directory\n", target_name);
            return;
        }
        if (file_node->type != TYPE_FILE) {
            printf("cat: '%s': Is a directory\n", target_name);
            return;
        }
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
        } else {
            printf("%s", file_node->data);
            if (strlen(file_node->data) > 0 &&
                file_node->data[strlen(file_node->data) - 1] != '\n') {
                printf("\n");
            }
        }
    }
}
