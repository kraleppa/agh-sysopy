#include <fcntl.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "define.h"

mqd_t server_queue;
int ID_COUNT = 0;
int CLIENTS_COUNT = 0;
Client* clients[MAX_CLIENTS];

Client* getClient(int id) {
    for (int i = 0; i < CLIENTS_COUNT; i++) {
        if (clients[i]->id == id){
            return clients[i];
        } 
    }
    return NULL;
}



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

void initHandler(char *text){
    mqd_t queueId = mq_open(text, O_RDWR, 0666, NULL);
    Client* newClient = calloc(1, sizeof(Client));
    newClient->queueFilename = calloc(FILENAME_LEN + 1, sizeof(Client));

    newClient -> id = ID_COUNT;
    ID_COUNT++;

    newClient -> queueId = queueId;
    sprintf(newClient -> queueFilename, "%s", text);

    newClient -> connectedClientId = -1;
    clients[CLIENTS_COUNT] = newClient;
    CLIENTS_COUNT++;
    char reply[TEXT_LEN + 1];
    sprintf(reply, "%d", newClient->id);

    sendMessage(newClient->queueId, INIT, reply);
}

void listHandler(char* text){
    int clientId = atoi(strtok(text, " "));
    int secondId = atoi(strtok(NULL, " "));

    Client* first = getClient(clientId);
    Client* second = getClient(secondId);

    first -> connectedClientId = second -> id;
    second -> connectedClientId = first -> id;

    char reply[TEXT_LEN + 1];

    sprintf(reply, "%s", first -> queueFilename);
    sendMessage(second->queueId, CONNECT, reply);
    sprintf(reply, "%s", second -> queueFilename);
    sendMessage(first -> queueId, CONNECT, reply);
}

void disconnectHandler(char* text) {
    int clientId = atoi(text);

    Client* first = getClient(clientId);
    Client* second = getClient(first->connectedClientId);

    first -> connectedClientId = -1;
    second -> connectedClientId = -1;

    char reply[TEXT_LEN + 1];
    sendMessage(second -> queueId, DISCONNECT, reply);
}

void stopHandler(char* text) {
    int clientId = atoi(text);

    int clientOffset;
    for (int i = 0; i < CLIENTS_COUNT; i++) {
        if (clients[i]->id == clientId) {
            clientOffset = i;
            break;
        }
    }

    Client* client_to_be_deleted = clients[clientOffset];

    for (int i = clientOffset; i < CLIENTS_COUNT - 1; i++) {
        clients[i] = clients[i + 1];
    }
    clients[CLIENTS_COUNT - 1] = NULL;
    CLIENTS_COUNT--;

    mq_close(client_to_be_deleted -> queueId);
    free(client_to_be_deleted -> queueFilename);
    free(client_to_be_deleted);
}

void connectHandler(char* text) {
    int client_id = atoi(strtok(text, " "));
    int second_id = atoi(strtok(NULL, " "));

    Client* first = getClient(client_id);
    Client* second = getClient(second_id);

    first->connectedClientId = second->id;
    second->connectedClientId = first->id;

    char reply[TEXT_LEN + 1] = {0};
    sprintf(reply, "%s", first->queueFilename);
    sendMessage(second->queueId, CONNECT, reply);
    sprintf(reply, "%s", second->queueFilename);
    sendMessage(first->queueId, CONNECT, reply);
}

void stopServer() {
    char text[TEXT_LEN + 1] = {0};
    for (int i = 0; i < CLIENTS_COUNT; i++) {
        sendMessage(clients[i]->queueId, STOP_SERVER, text);
    }

    while (CLIENTS_COUNT > 0) {
        char type;
        char* reply = readMessage(server_queue, &type);
        puts(reply);

        if (type == STOP) {
            stopHandler(reply);
        }

        free(reply);
    }

    mq_close(server_queue);
    mq_unlink("/server");
    exit(0);
}

void sigintHandler(int signum) {
    (void)signum;
    stopServer();
}

int main() {
    server_queue = mq_open("/server", O_RDWR | O_CREAT, 0666, NULL);

    signal(SIGINT, sigintHandler);

    while (1) {
        char type;
        char* text = readMessage(server_queue, &type);

        switch (type) {
            case INIT:
                initHandler(text);
                break;
            case LIST:
                listHandler(text);
                break;
            case CONNECT:
                connectHandler(text);
                break;
            case DISCONNECT:
                disconnectHandler(text);
                break;
            case STOP:
                stopHandler(text);
                break;
        }
        free(text);
    }

    stopServer();
}