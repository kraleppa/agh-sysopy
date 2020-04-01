#define _POSIX_C_SOURCE 199309L
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int count_sigusr1 = 0;
int start_signal_number;
char *mode;
void sigusr1_handle(int sig_no, siginfo_t *info, void *context){
    count_sigusr1++;
    if (strcmp(mode, "siqueue") == 0){
        printf("Signal number: %d\n", info -> si_value.sival_int);
    }
}

void sigusr2_handle(int sig_no, siginfo_t *info, void *context){
    printf("start signal number: %d\n", start_signal_number);
    printf("recieved number: %d\n", count_sigusr1);

    if (strcmp(mode, "siqueue") == 0){
        printf("Last signal number: %d\n", info -> si_value.sival_int);
    }
    exit(0);
}


//PID ILOŚĆ TRYB
int main(int argc, char *argv[]){
    if (argc != 4){
        perror("wrong syntax");
        exit(1);
    }

    int pid = atoi(argv[1]);
    int number = atoi(argv[2]);
    start_signal_number = number;
    mode = argv[3];

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




    if (strcmp(mode, "kill") == 0){
        for (int i = 0; i < number; i++){
            kill(pid, SIGUSR1);
        }
        
        kill(pid, SIGUSR2);
    }

    if (strcmp(mode, "siqueue") == 0){
        for (int i = 0; i < number; i++){
            sigqueue(pid, SIGUSR1, (union sigval){.sival_int = 0});
        }

        sigqueue(pid, SIGUSR2, (union sigval){.sival_int = 0});
    }

    if (strcmp(mode, "sigrt") == 0){
        for (int i = 0; i < number; i++){
            kill(pid, SIGRTMIN);
        }
        
        kill(pid, SIGRTMAX);
    }


    while(1){
        ;
    }
}