#define _POSIX_C_SOURCE 1
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

void handler(int sig_no){
    printf("Odebrano SIGUSER\n");
}

void check_pending(){
    sigset_t set;
    sigpending(&set);
    if (sigismember(&set, SIGUSR1) == 1){
        printf("SIGUSR1 is pending\n");
    }    
}

int main(int argc, char *argv[]){
    int is_exec;
    if (argc != 2 && argc != 3){
        perror("wrong syntax");
        exit(1);
    }
    if (argc == 3){
        is_exec = 1;
    }

    char *mode = argv[1];

    if (strcmp(mode, "ignore") == 0){
        struct sigaction action;
        action.sa_flags = 0;
        action.sa_handler = SIG_IGN;
        sigaction(SIGUSR1, &action, NULL);
    }
    
    if (strcmp(mode, "handler") == 0){
        struct sigaction action;
        action.sa_flags = 0;
        action.sa_handler = handler;
        sigaction(SIGUSR1, &action, NULL);
    }

    if (strcmp(mode, "mask") == 0 || strcmp(mode, "pending") == 0){
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &mask, NULL);
    }


    
    printf("inside parent: \n");

    raise(SIGUSR1);
    if (strcmp(mode, "mask") == 0 || strcmp(mode, "pending") == 0){
        check_pending();
    }

    if (is_exec == 1) {
        execl("./child", "./child", mode, NULL);
    } else {
        pid_t child_pid = fork();
        if (child_pid == 0){
            printf("inside child: \n");
            if (strcmp(mode, "pending") != 0){
                raise(SIGUSR1);
            }

            if (strcmp(mode, "mask") == 0 || strcmp(mode, "pending") == 0){
                check_pending();
            }
        }   
    }
    return 0;
}