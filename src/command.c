#include <stdio.h>
#include <string.h>
#include "command.h"

void cmd_vstat(ParsedCommand* parsed);
void cmd_ls(ParsedCommand* parsed);
void cmd_cd(ParsedCommand* parsed);
void cmd_mkdir(ParsedCommand* parsed);
void cmd_cat(ParsedCommand* parsed);
void cmd_pwd(ParsedCommand* parsed);
void cmd_rmdir(ParsedCommand* parsed);
void cmd_touch(ParsedCommand* parsed);
void cmd_cp(ParsedCommand* parsed);
void cmd_chmod(ParsedCommand* parsed);
void cmd_clear(ParsedCommand* parsed);
void cmd_wc(ParsedCommand* parsed);

void dispatch_command(ParsedCommand* parsed) {
    if (strcmp(parsed->command, "vstat") == 0)  { cmd_vstat(parsed); return; }
    if (strcmp(parsed->command, "ls") == 0)     { cmd_ls(parsed);    return; }
    if (strcmp(parsed->command, "cd") == 0)     { cmd_cd(parsed);    return; }
    if (strcmp(parsed->command, "mkdir") == 0)  { cmd_mkdir(parsed); return; }
    if (strcmp(parsed->command, "cat") == 0)    { cmd_cat(parsed);   return; }
    if (strcmp(parsed->command, "pwd") == 0)    { cmd_pwd(parsed);   return; }
    if (strcmp(parsed->command, "rmdir") == 0)  { cmd_rmdir(parsed); return; }
    if (strcmp(parsed->command, "touch") == 0)  { cmd_touch(parsed); return; }
    if (strcmp(parsed->command, "cp") == 0)     { cmd_cp(parsed);    return; }
    if (strcmp(parsed->command, "chmod") == 0)  { cmd_chmod(parsed); return; }
    if (strcmp(parsed->command, "clear") == 0)  { cmd_clear(parsed); return; }
    if (strcmp(parsed->command, "wc") == 0)     { cmd_wc(parsed);    return; }

    printf("Unknown command: %s\n", parsed->command);
}
