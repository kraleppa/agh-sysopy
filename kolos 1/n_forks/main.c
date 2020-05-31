#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int main ( int argc, char *argv[] )
{
    int i;
    printf("I am parent and my PID is: %d\n", (int)getpid());
    if(argc != 2){
        printf("Not a suitable number of program arguments");
        exit(2);
    }else {
       for (i = 0; i < atoi(argv[1]); i++) {
        //*********************************************************
        //Uzupelnij petle w taki sposob aby stworzyc dokladnie argv[1] procesow potomnych, bedacych dziecmi
        //   tego samego procesu macierzystego.
           // Kazdy proces potomny powinien:
               // - "powiedziec ktorym jest dzieckiem",
                //-  jaki ma pid,
                //- kto jest jego rodzicem
           //******************************************************
           if (fork() == 0){
               printf("I am %d child! My PID is %d! My parent is %d\n", i, (int)getpid(), (int)getppid());
               exit(0);
           }
        }
    }

    for (int j = 0; j < atoi(argv[1]); j++){
        wait(NULL);
    }
    return 0;
}