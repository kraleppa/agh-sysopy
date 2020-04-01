#define _POSIX_C_SOURCE 199309L
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int sigusr1_count = 0;
char *mode;
void sigusr1_handle(int sig_no, siginfo_t *info, void *context){
    sigusr1_count++;
}

void sigusr2_handle(int sig_no, siginfo_t *info, void *context){
    int sender_pid = info -> si_pid;
    printf("recieved number: %d\n", sigusr1_count);

    if (strcmp(mode, "kill") == 0){
        for (int i = 0; i < sigusr1_count; i++){
            kill(sender_pid, SIGUSR1);
        }

        kill(sender_pid, SIGUSR2);
    }

    if (strcmp(mode, "siqueue") == 0){
        for (int i = 0; i < sigusr1_count; i++){
            sigqueue(sender_pid, SIGUSR1, (union sigval){.sival_int = i});
        }

        sigqueue(sender_pid, SIGUSR2, (union sigval){.sival_int = sigusr1_count});
    } 

    if (strcmp(mode, "sigrt") == 0){
        
        for (int i = 0; i < sigusr1_count; i++){
            kill(sender_pid, SIGRTMIN);
        }
        kill(sender_pid, SIGRTMAX);
    }



    exit(0);
}

int main(int argc, char *argv[]){
    if (argc != 2){
        perror("catcher wrong syntax");
        exit(1);
    }

    mode = argv[1];

    printf("%d\n", getpid());

    sigset_t block_mask;
    sigfillset(&block_mask);
    sigdelset(&block_mask, SIGUSR1);
    sigdelset(&block_mask, SIGUSR2);
    sigdelset(&block_mask, SIGRTMIN);
    sigdelset(&block_mask, SIGRTMAX);
    sigprocmask(SIG_SETMASK, &block_mask, NULL);

    struct sigaction sigusr1;
    sigusr1.sa_sigaction = sigusr1_handle;
    sigusr1.sa_flags = SA_SIGINFO;
    sigemptyset(&sigusr1.sa_mask);
    sigaction(SIGUSR1, &sigusr1, NULL);

    struct sigaction sigusr2;
    sigusr2.sa_sigaction = sigusr2_handle;
    sigusr2.sa_flags = SA_SIGINFO;
    sigemptyset(&sigusr2.sa_mask);
    sigaction(SIGUSR2, &sigusr2, NULL);





    //////realtime
    struct sigaction sigrtmin0;
    sigrtmin0.sa_sigaction = sigusr1_handle;
    sigrtmin0.sa_flags = SA_SIGINFO;
    sigemptyset(&sigrtmin0.sa_mask);
    sigaction(SIGRTMIN, &sigrtmin0, NULL);

    struct sigaction sigrtmin1;
    sigrtmin1.sa_sigaction = sigusr2_handle;
    sigrtmin1.sa_flags = SA_SIGINFO;
    sigemptyset(&sigrtmin1.sa_mask);
    sigaction(SIGRTMAX, &sigrtmin1, NULL);




    while(1){
        ;
    }

}