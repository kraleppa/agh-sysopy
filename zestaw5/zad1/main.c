#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>


#define MAX_LINE_LENGTH 10000
#define MAX_ARGS 3
#define MAX_CMDS 3

typedef struct {
    char** args;
} command;

void free_command(command* cmd) {
    for (char** arg = cmd->args; *arg != NULL; arg++) {
        free(*arg);
    }
    free(cmd->args);
}

void strip_ending_whitespace(char* s) {
    int i = strlen(s) - 1;
    while (i >= 0 && isspace(s[i])) {
        s[i] = '\0';
        i--;
    }
}

int main(int argc, char* argv[]){
    if (argc != 2){
        perror("wrong syntax");
        exit(1);
    }

    FILE* file = fopen(argv[1], "r");

    char line[MAX_LINE_LENGTH + 1];
    
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL){
        strip_ending_whitespace(line);
        printf("%s\n", line);
        
        command* commands[MAX_CMDS] = {NULL};
        int c_index = 0;

        char *cmd = strtok(line, "|");
        while (cmd != NULL){
            char** args = calloc(MAX_ARGS + 2, sizeof(char *));
            char* buffer;
            char* token = strtok_r(cmd, " ", &buffer);
            args[0] = strdup(token);
            token = strtok_r(NULL, " ", &buffer);
            int i = 1;
            while (token != NULL){
                args[i++] = strdup(token);
                token = strtok_r(NULL, " ", &buffer);
            }

            command* comm = calloc(1, sizeof(command));
            comm -> args = args;
            commands[c_index++] = comm;

            cmd = strtok(NULL, "|"); 
        }

        int pipes[2], prev_pipes[2];
        int i;
        for (i = 0; commands[i] != NULL; i++){
            pipe(pipes);

            pid_t pid = fork();
            if (pid == 0){
                if (i != 0){
                    dup2(prev_pipes[0], STDIN_FILENO);
                    close(prev_pipes[1]);
                }
                dup2(pipes[1], STDOUT_FILENO);
                execvp(commands[i]->args[0], commands[i]->args);
            }
            close(pipes[1]);

            prev_pipes[0] = pipes[0];
            prev_pipes[1] = pipes[1];

            free_command(commands[i]);
            free(commands[i]);

        }

        while (i-- > 0) {
            wait(NULL);
        }

        char buffer[257] = {0};
        while (read(pipes[0], buffer, sizeof(buffer) - 1) > 0) {
            printf("%s", buffer);
            memset(buffer, 0, sizeof(buffer));
        }
        puts("");

    }

    fclose(file);
}