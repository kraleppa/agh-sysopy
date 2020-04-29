#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define RECIEVERS_COUNT 7
#define BAGGERS_COUNT 5
#define SENDERS_COUNT 3
#define PACKAGES_COUNT 10
#define MAX_CREATED_COUNT 10

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

pid_t childs[RECIEVERS_COUNT + BAGGERS_COUNT + SENDERS_COUNT];

void sigint_handler(int signum) {
    for (int i = 0; i < RECIEVERS_COUNT + BAGGERS_COUNT + SENDERS_COUNT; i++) {
        kill(childs[i], SIGINT);
    }
}

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


sem_t* created;
sem_t* space;
sem_t* can_modify;
sem_t* packed;
int mem;

int main(){
    signal(SIGINT, sigint_handler);
    created = sem_open("/created", O_CREAT | O_RDWR, 0666, 0);
    space = sem_open("/space", O_CREAT | O_RDWR, 0666, PACKAGES_COUNT);
    can_modify = sem_open("/can_modify", O_CREAT | O_RDWR, 0666, 1);
    packed = sem_open("/packed", O_CREAT | O_RDWR, 0666, 0);
    mem = shm_open("/memory", O_CREAT | O_RDWR, 0666);
    ftruncate(mem, sizeof(memory_t));

    memory_t *m = mmap(NULL, sizeof(memory_t), PROT_WRITE, MAP_SHARED, mem, 0);
    m -> index = -1;
    m -> size = 0;
    for (int i = 0; i < PACKAGES_COUNT; i++) {
        m->packages[i].status = SENT;
        m->packages[i].value = 0;
    }
    munmap(m, sizeof(memory_t));
    fork_recievers();
    fork_baggers();
    fork_senders();

    for (int i = 0; i < RECIEVERS_COUNT + BAGGERS_COUNT + SENDERS_COUNT; i++) {
        wait(0);
    }

    sem_close(space);
    sem_close(created);
    sem_close(packed);
    sem_close(can_modify);
    sem_unlink("/space");
    sem_unlink("/created");
    sem_unlink("/packed");
    sem_unlink("/can_modify");
    shm_unlink("/memory");

    exit(0);

}
