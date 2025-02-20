#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{ 
   char input_buffer[SH_CMD_MAX];
   cmd_buff_t cmd;
   int last_exit_status = 0;  

   while (1) {
    printf("%s", SH_PROMPT);
    if (fgets(input_buffer, SH_CMD_MAX, stdin) == NULL) {
        printf("\n");
        break;
    }
    input_buffer[strcspn(input_buffer, "\n")] = '\0';

    if (strlen(input_buffer) == 0) {
        printf(CMD_WARN_NO_CMD);
        continue;
    }

   
    if (strcmp(input_buffer, "echo $?") == 0) {
        printf("%d\n", last_exit_status);
        continue;
    }

    if (build_cmd_buff(input_buffer, &cmd) != OK) {
        last_exit_status = WARN_NO_CMDS;
        continue;
    }

    Built_In_Cmds cmd_type = match_command(cmd.argv[0]);
    if (cmd_type != BI_NOT_BI) {
        last_exit_status = exec_built_in_cmd(&cmd);
        if ((int)last_exit_status == (int)OK_EXIT) {
            break;
        }
        continue;
    }

    last_exit_status = exec_cmd(&cmd);
}
    return OK;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (strcmp(cmd->argv[0], "exit") == 0) {
        return OK_EXIT;
    }
    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc == 1) {
            return OK;
        }
        if (chdir(cmd->argv[1]) != 0) {
            perror("cd failed");
            return ERR_EXEC_CMD;
        }
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}


int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return ERR_EXEC_CMD;
    } else if (pid == 0) {
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp failed");  
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);  
        }
    }

    return ERR_EXEC_CMD;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = strdup(cmd_line);
    if (!cmd_buff->_cmd_buffer) return ERR_MEMORY;

    cmd_buff->argc = 0;
    char *ptr = cmd_buff->_cmd_buffer;
    bool in_quotes = false;

    while (*ptr) {
        while (*ptr == ' ' && !in_quotes) ptr++;

        if (*ptr == '"') {  
            in_quotes = !in_quotes;
            ptr++;  
        }

        if (*ptr == '\0') break;

        cmd_buff->argv[cmd_buff->argc++] = ptr;

        while (*ptr && (in_quotes || *ptr != ' ')) {
            if (*ptr == '"' && in_quotes) {
                *ptr = '\0';
                in_quotes = false;
                break;
            }
            ptr++;
        }

        if (*ptr) {
            *ptr = '\0';
            ptr++;
        }
    }

    cmd_buff->argv[cmd_buff->argc] = NULL;
    return (cmd_buff->argc > 0) ? OK : WARN_NO_CMDS;
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, "exit") == 0) {
        return BI_CMD_EXIT;
    }
    if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    }
    return BI_NOT_BI;
}
