#define _POSIX_C_SOURCE 200809L
#include <fenv.h>
#include <limits.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

void child_handle(int sig_no, siginfo_t *info, void *context) {
    printf("Process %d exited with signal %d\n", info -> si_pid, info -> si_status);
    exit(0);
}

void segfault_handle(int sig_no, siginfo_t *info, void *context){
    printf("Address of faulting memory: %p\n", info -> si_addr);
    exit(0);
}

void user_handle(int sig_no, siginfo_t *info, void *context){
    printf("%d", info -> si_value.sival_int);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2){
        perror("wrong syntax");
        exit(1);
    }

    int mode = atoi(argv[1]);
    if (mode == 1){
        struct sigaction action;
        action.sa_sigaction = child_handle;
        action.sa_flags = SA_SIGINFO;
        sigemptyset(&action.sa_mask);
        sigaction(SIGCHLD , &action, NULL);

        srand(time(0)); 

        pid_t child = fork();
        if (child == 0){
            printf("inside child\n");
            exit(rand() % 100);
        } else{
            pause();
        }
        return 0;
    }

    if (mode == 2){
        struct sigaction action;
        action.sa_sigaction = segfault_handle;
        action.sa_flags = SA_SIGINFO;
        sigemptyset(&action.sa_mask);
        sigaction(SIGSEGV , &action, NULL);

        char *s = "hello world";
        *s = 'H';
        return 0;
    }

    if (mode == 3){
        struct sigaction action;
        action.sa_sigaction = user_handle;
        action.sa_flags = SA_SIGINFO;
        sigemptyset(&action.sa_mask);
        sigaction(SIGUSR1 , &action, NULL);

        srand(time(0)); 

        sigqueue(getpid(), SIGUSR1, (union sigval){.sival_int = rand() % 100});
    }


    return 0;
}

