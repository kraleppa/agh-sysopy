#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "define.h"

int clientId;
mqd_t clientQueue;
mqd_t serverQueue;
mqd_t partnerQueue;

char fileName[FILENAME_LEN + 1];

char* readMessage(mqd_t src, char* type) {
    char fromQueue[TEXT_LEN + 2] = {0};

    int success = mq_receive(src, fromQueue, TEXT_LEN + 1, NULL);
    if (success == -1){
        return NULL;
    } 

    if (type) {
        *type = fromQueue[0];
    }
    char* retval = calloc(TEXT_LEN + 1, sizeof(char));
    sprintf(retval, "%s", fromQueue + 1);

    return retval;
}

void sendMessage(mqd_t dest, char type, char* message) {
    int length = strlen(message);
    char* buffer = calloc(2 + length, sizeof(char));
    buffer[0] = type;
    sprintf(buffer + 1, "%s", message);
    mq_send(dest, buffer, length + 1, TYPES_COUNT - type + 1);
}

void stopClient() {
    char msg[TEXT_LEN + 1] = {0};
    sprintf(msg, "%d", clientId);

    sendMessage(serverQueue, STOP, msg);

    puts("Deleting queue...");
    mq_unlink(fileName);
    exit(0);
}


void registerNotification();

void notificationHandler(union sigval sv){

    registerNotification();
    char *text;
    char type;
    while ((text = readMessage(clientQueue, &type)) != NULL){
        if (type == CONNECT){
            partnerQueue = mq_open(text, O_RDWR, 0666, NULL);
            continue;
        }

        if (type == SEND){
            printf("MESSAGE: %s", text);
            continue;
        }

        if (type == DISCONNECT){
            if (partnerQueue){
                mq_close(partnerQueue);
            }
            partnerQueue = -1;
            continue;
        }

        if (type == STOP_SERVER){
            stopClient();
            continue;
        }

        puts(text);
    }
}

void registerNotification() {
    struct sigevent event;

    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = notificationHandler;
    event.sigev_notify_attributes = NULL;
    event.sigev_value.sival_ptr = NULL;

    mq_notify(clientQueue, &event);
}

void setNonblocking(){
    struct mq_attr attr;
    mq_getattr(clientQueue, &attr);
    attr.mq_flags = O_NONBLOCK;
    mq_setattr(clientQueue, &attr, NULL);
}

int startsWith(char *s, char *prefix) {
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

void sigintHandler() { stopClient(); }

int main() {
    sprintf(fileName, "/%d", getpid());
    clientQueue = mq_open(fileName, O_RDWR | O_CREAT, 0666, NULL);

    serverQueue = mq_open("/server", O_RDWR, 0666, NULL);

    signal(SIGINT, sigintHandler);

    sendMessage(serverQueue, INIT, fileName);
    
    char *encodedId = readMessage(clientQueue, NULL);

    clientId = atoi(encodedId);
    free(encodedId);

    setNonblocking();
    registerNotification();
    char line[128];

    while (fgets(line, sizeof(line), stdin)){
        char text[TEXT_LEN + 1] = {0};
        int type = -1;
        int is_msg_to_client = 0;

         if (startsWith(line, "LIST")) {
            type = LIST;
            sprintf(text, "%d", clientId);
        }

        if (startsWith(line, "CONNECT")) {
            type = CONNECT;

            (void)strtok(line, " ");
            int secondId = atoi(strtok(NULL, " "));
            sprintf(text, "%d %d", clientId, secondId);
        }

        if (startsWith(line, "SEND") && partnerQueue != -1) {
            type = SEND;

            sprintf(text, "%s", strchr(line, ' ') + 1);
            is_msg_to_client = 1;
        }

        if (startsWith(line, "DISCONNECT")) {
            type = DISCONNECT;
            sprintf(text, "%d", clientId);
            partnerQueue = -1;
        }

        if (startsWith(line, "STOP")) {
            stopClient();
        }

        if (type != -1) {
            mqd_t destination = is_msg_to_client ? partnerQueue : serverQueue;
            sendMessage(destination, type, text);

            sleep(1);
        }
    }

    stopClient();
}