 #define _POSIX_C_SOURCE 199309L
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "define.h"

int clientQueue;
int serverQueue;
int clientId;
int partnerQueue = -1;

int startsWith(char *s, char *prefix) {
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

void stopClient(){
    message msg;
    msg.type = STOP;
    sprintf(msg.text, "%d", clientId);

    msgsnd(serverQueue, &msg, TEXT_LEN, 0);

    msgctl(clientQueue, IPC_RMID, NULL);
    exit(0);
}

void getReplies(union sigval sv){
    (void)sv;
    message reply;
    while (msgrcv(clientQueue, &reply, TEXT_LEN, -TYPES_COUNT, IPC_NOWAIT) != -1){
        if (reply.type == CONNECT){
            partnerQueue = atoi(reply.text);
            continue;
        }

        if (reply.type == SEND){
            printf("MESSAGE: %s", reply.text);
            continue;
        }

        if (reply.type == DISCONNECT) {
            partnerQueue = -1;
            continue;
        }

        if (reply.type == STOP_SERVER) {
            stopClient();
            continue;
        } 

        puts(reply.text);
    }
}

void sigintHandler(int signum) { 
    stopClient(); 
}

void setTimer() {
    timer_t timer;
    struct sigevent event;
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = getReplies;
    event.sigev_notify_attributes = NULL;
    event.sigev_value.sival_ptr = NULL;
    timer_create(CLOCK_REALTIME, &event, &timer);
    struct timespec ten_ms = {0, 10000000};
    struct itimerspec timer_value = {ten_ms, ten_ms};
    timer_settime(timer, 0, &timer_value, NULL);
}

int main() {

    char* homePath = getpwuid(getuid())->pw_dir;

    serverQueue = msgget(ftok(homePath, SERVER_KEY_ID), 0666);
    clientQueue = msgget(ftok(homePath, getpid()), IPC_CREAT | 0666);

    signal(SIGINT, sigintHandler);

    message msgInit;
    msgInit.type = INIT;
    sprintf(msgInit.text, "%d", clientQueue);
    msgsnd(serverQueue, &msgInit, TEXT_LEN, 0);

    message initAnserw;
    msgrcv(clientQueue, &initAnserw, TEXT_LEN, INIT, 0);

    clientId = atoi(initAnserw.text);

    char line[128];
    setTimer();

    while (fgets(line, sizeof(line), stdin)){
        message msg;
        msg.type = -1;
        int is_msg_to_client = 0;

        if (startsWith(line, "LIST")) {
            msg.type = LIST;
            sprintf(msg.text, "%d", clientId);
        }

        if (startsWith(line, "CONNECT")) {
            msg.type = CONNECT;

            (void)strtok(line, " ");
            int second_id = atoi(strtok(NULL, " "));
            sprintf(msg.text, "%d %d", clientId, second_id);
        }

        if (startsWith(line, "SEND") && partnerQueue != -1){
            msg.type = SEND;

            sprintf(msg.text, "%s", strchr(line, ' ') + 1);
            is_msg_to_client = 1;
        }

        if (startsWith(line, "DISCONNECT")) {
            msg.type = DISCONNECT;
            sprintf(msg.text, "%d", clientId);
            partnerQueue = -1;
        }

        if (startsWith(line, "STOP")) {
            stopClient();
        }

        if (msg.type != -1) {
            int destination = is_msg_to_client ? partnerQueue : serverQueue;
            msgsnd(destination, &msg, TEXT_LEN, 0);
        }
    }

    stopClient();
}