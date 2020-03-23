#define _XOPEN_SOURCE 500

#include <ftw.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

int display_info(const char *path, const struct stat *sb, int flagType, struct FTW *tw_buf){
    char *absPath = (char*) calloc(150, sizeof(char));

    realpath(path, absPath);
    pid_t pid = fork();
    if (pid == 0){
        if (flagType == FTW_D){
            printf("PID:       %d\n", getpid());
            printf("Path:      %s\n", path);
            char cmd[80] = "ls -l ";
            strcat(cmd, path);
            system(cmd);
            printf("\n");
        }
        exit(0);
    }
    else{
        wait(NULL);
    }
    return 0;
}

int main(int argc, char **argv) {
    char * path;
    if (argc != 2)
        path = ".";
    else
        path = argv[1];

    nftw(path, display_info, 10, FTW_PHYS);

    return 0;
}

