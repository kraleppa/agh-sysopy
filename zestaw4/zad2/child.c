#define _POSIX_C_SOURCE 1
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void check_pending(){
    sigset_t set;
    sigpending(&set);
    if (sigismember(&set, SIGUSR1) == 1){
        printf("SIGUSR1 is pending\n");
    }    
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        perror("wrong syntax");
        exit(1);
    }

    char *mode = argv[1];
    printf("inside e-child: \n");
    if (strcmp(mode, "pending") != 0){
        raise(SIGUSR1);
    }

    if (strcmp(mode, "mask") == 0 || strcmp(mode, "pending") == 0){
        check_pending();
    }
}