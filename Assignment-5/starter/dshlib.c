#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"


int prev_exit_code = 0;

int execute_commands(command_list_t *cmd_list);
int parse_and_run_pipeline(char *input);
int parse_command_sequence(char *input, command_list_t *cmd_list);
void release_pipeline(command_list_t *cmd_list);
int alloc_cmd_buff(cmd_buff_t *cmd_buff);
int free_cmd_buff(cmd_buff_t *cmd_buff);
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff);
int clear_cmd_buff(cmd_buff_t *cmd_buff);

int exec_local_cmd_loop() {
    cmd_buff_t command_buffer;
    int rc;

    if ((rc = alloc_cmd_buff(&command_buffer)) != OK) {
        fprintf(stderr, "Memory allocation failed\n");
        return ERR_MEMORY;
    }

    bool first_iteration = true;
    while (1) {
        if (!first_iteration) {
            printf("%s", SH_PROMPT);
            fflush(stdout);
        }
        first_iteration = false;

        if (fgets(command_buffer._cmd_buffer, SH_CMD_MAX, stdin) == NULL) {
            break;
        }

        command_buffer._cmd_buffer[strcspn(command_buffer._cmd_buffer, "\n")] = '\0';
        char *trimmed = command_buffer._cmd_buffer;
        while (*trimmed && isspace((unsigned char)*trimmed))
            trimmed++;

        if (*trimmed == '\0') {
             printf("%s", CMD_WARN_NO_CMD);
	    continue;
        }

        if (strcmp(trimmed, EXIT_CMD) == 0)
            break;

        rc = parse_and_run_pipeline(trimmed);
        if (rc == OK_EXIT) {
            break;
        }
    }

    printf("%s", SH_PROMPT);
    fflush(stdout);

    free_cmd_buff(&command_buffer);
    return OK;
}

int parse_and_run_pipeline(char *input) {
    command_list_t pipeline;
    int rc = parse_command_sequence(input, &pipeline);
    if (rc != OK) {
        fprintf(stderr, "Error parsing command: %d\n", rc);
        return rc;
    }

    rc = execute_commands(&pipeline);
    release_pipeline(&pipeline);
    return rc;
}

int parse_command_sequence(char *input, command_list_t *cmd_list) {
    cmd_list->num = 0;

    char *saveptr1;
    char *token = strtok_r(input, PIPE_STRING, &saveptr1);
    while (token != NULL && cmd_list->num < CMD_MAX) {
    while (*token && isspace((unsigned char)*token)) token++;

    if (alloc_cmd_buff(&cmd_list->commands[cmd_list->num]) != OK) {
        return ERR_MEMORY;
    }

    if (build_cmd_buff(token, &cmd_list->commands[cmd_list->num]) != OK) {
        return ERR_CMD_ARGS_BAD;
    }

    cmd_list->num++;
    token = strtok_r(NULL, PIPE_STRING, &saveptr1);
}
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    char *saveptr2;
    cmd_buff->argc = 0;
    char *token = strtok_r(cmd_line, " ", &saveptr2);

    while (token != NULL && cmd_buff->argc < CMD_ARGV_MAX - 1) {
    cmd_buff->argv[cmd_buff->argc] = token;
    cmd_buff->argc++;
    token = strtok_r(NULL, " ", &saveptr2);
    }

cmd_buff->argv[cmd_buff->argc] = NULL;
    return OK;
}

int execute_commands(command_list_t *cmd_list) {
    if (cmd_list->num == 0) {
        return OK;
    }

    pid_t *pids = malloc(cmd_list->num * sizeof(pid_t));
    if (pids == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return ERR_MEMORY;
    }

    int pipes[CMD_MAX - 1][2];
    for (int i = 0; i < cmd_list->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            free(pids);
            return ERR_EXEC_CMD;
        }
    }

    for (int i = 0; i < cmd_list->num; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            free(pids);
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0) {
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            if (i < cmd_list->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < cmd_list->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(cmd_list->commands[i].argv[0], cmd_list->commands[i].argv);
            perror("Execution failed");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < cmd_list->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    int last_status = 0;
    for (int i = 0; i < cmd_list->num; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (i == cmd_list->num - 1) {
            last_status = WEXITSTATUS(status);
            prev_exit_code = last_status;
        }
    }

    free(pids);
    return OK;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (!cmd_buff->_cmd_buffer) return ERR_MEMORY;
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer) free(cmd_buff->_cmd_buffer);
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    return OK;
}

void release_pipeline(command_list_t *cmd_list) {
    for (int i = 0; i < cmd_list->num; i++) {
        clear_cmd_buff(&cmd_list->commands[i]);
    }
    cmd_list->num = 0;
}
