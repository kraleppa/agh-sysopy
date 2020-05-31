#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sighandler(int sig_no, siginfo_t *info, void *context){
    printf("recieved value %d, my pid is %d\n", info -> si_value.sival_int, getpid());
    exit(0);
}


int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;

    sigset_t set;
    sigfillset(&set);
    sigdelset(&set, SIGUSR1);
    action.sa_mask = set;

    action.sa_flags = SA_SIGINFO;
    //..........


    int child = fork();
    if(child == 0) {
        //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1
        //zdefiniuj obsluge SIGUSR1 w taki sposob zeby proces potomny wydrukowal ok
        //na konsole przekazana przez rodzica wraz z sygnalem SIGUSR1 wartosc ok
        sigprocmask(SIG_BLOCK, &set, NULL);
        sigaction(SIGUSR1, &action, NULL);
        while(1){
            ;
        }
    }
    else {
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]
        sleep(1);
        printf("I am parent and my pid is: %d, child pid is: %d\n", getpid(), child);
        union sigval sv;
        sv.sival_int = atoi(argv[1]);
        sigqueue(child, atoi(argv[2]), sv);
    }

    return 0;
}