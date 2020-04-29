 #define _POSIX_C_SOURCE 1
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define RECIEVERS_COUNT 7
#define SPACE_INDEX 0
#define CREATED_INDEX 1
#define PACKED_INDEX 2
#define CAN_MODIFY_INDEX 3
#define BAGGERS_COUNT 5
#define PACKAGES_COUNT 10
#define SENDERS_COUNT 3

typedef enum { CREATED, PACKED, SENT } package_status;

typedef struct {
    package_status status;
    int value;
} package_t;

typedef struct {
    int index;
    int size;
    package_t packages[PACKAGES_COUNT];
} memory_t;





int mem = -1;
int semaphores_set = -1;
pid_t childs[RECIEVERS_COUNT + BAGGERS_COUNT + SENDERS_COUNT];

void fork_recievers(){
    int j = 0;
    for (int i = 0; i < RECIEVERS_COUNT; i++) {
        childs[j] = fork();
        if (childs[j] == 0) {
            execlp("./reciever", "./reciever", NULL);
            return;
        }
        j++;
    }
}

void fork_baggers(){
    int j = 0;
    for (int i = 0; i < BAGGERS_COUNT; i++) {
        childs[j] = fork();
        if (childs[j] == 0) {
            execlp("./bagger", "./bagger", NULL);
            perror("test");
            return;
        }
        j++;
    }
}

void fork_senders(){
    int j = 0;
    for (int i = 0; i < SENDERS_COUNT; i++) {
        childs[j] = fork();
        if (childs[j] == 0) {
            execlp("./sender", "./sender", NULL);
            return;
        }
        j++;
    }
}

void sigint_handler(int signumber){
    printf("killing children... :<");
    for (int i = 0; i < RECIEVERS_COUNT + BAGGERS_COUNT + SENDERS_COUNT; i++) {
        kill(childs[i], SIGINT);
    }
}

int main(){
    signal(SIGINT, sigint_handler);
    key_t key = ftok("main", 1);
    semaphores_set = semget(key, 4, IPC_CREAT | 0666);
    mem = shmget(key, sizeof(memory_t), IPC_CREAT | 0666);


    semctl(semaphores_set, SPACE_INDEX, SETVAL, PACKAGES_COUNT);
    semctl(semaphores_set, CREATED_INDEX, SETVAL, 0);
    semctl(semaphores_set, PACKED_INDEX, SETVAL, 0);
    semctl(semaphores_set, CAN_MODIFY_INDEX, SETVAL, 1);

    memory_t* m = shmat(mem, NULL, 0);
    m -> index = -1;
    m -> size = 0;
    for (int i = 0; i < PACKAGES_COUNT; i++) {
        m -> packages[i].status = SENT;
        m -> packages[i].value = 0;
    }
    shmdt(m);
    fork_recievers();
    fork_baggers();
    fork_senders();
    for (int i = 0; i < RECIEVERS_COUNT + BAGGERS_COUNT + SENDERS_COUNT; i++) {
        wait(0);
    }

    if (semaphores_set != -1) {
        semctl(semaphores_set, 0, IPC_RMID);
    }
    if (mem != -1) {
        shmctl(mem, IPC_RMID, NULL);
    }

    return 0;
}
