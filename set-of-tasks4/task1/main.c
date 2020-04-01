#define _POSIX_C_SOURCE 1
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int paused = 0;
void sigstop_handler(int sig_no){
    if (!paused){
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu\n"); 
        paused = 1;
    } else {
        paused = 0;
    }
}

void sigint_handler(int sig_no){
    if (paused){
        printf("\nOdebrano sygnał SIGINT\n");
        exit(0);
    }
}


int main(){
    struct sigaction act; 
    act.sa_handler = sigstop_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGTSTP, &act, NULL);


    signal(SIGINT, sigint_handler);
    while (1){
        if (!paused){
            system("ls -l");
            sleep(1);
        }
    }
}