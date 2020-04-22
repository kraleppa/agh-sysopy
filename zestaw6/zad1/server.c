#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "define.h"

int serverQueue;
int ID_COUNT = 0;
int CLIENT_COUNT = 0;
struct Client* clients[MAX_CLIENTS];

struct Client* getClient(int id) {
    for (int i = 0; i < CLIENT_COUNT; i++) {
        if (clients[i]->id == id){
            return clients[i];
        } 
    }
    return NULL;
}

void stopHandler(message* msg) {
    int clientId = atoi(msg->text);

    int client_offset;
    for (int i = 0; i < CLIENT_COUNT; i++) {
        if (clients[i]->id == clientId) {
            client_offset = i;
            break;
        }
    }
    
    struct Client* client_to_be_deleted = clients[client_offset];

    for (int i = client_offset; i < CLIENT_COUNT - 1; i++) {
        clients[i] = clients[i + 1];
    }
    clients[CLIENT_COUNT - 1] = NULL;
    CLIENT_COUNT--;

    free(client_to_be_deleted);
}

void stopServer() {
    message stopServer;
    stopServer.type = STOP_SERVER;
    for (int i = 0; i < CLIENT_COUNT; i++) {
        msgsnd(clients[i]->queueId, &stopServer, TEXT_LEN, 0);
    }

    printf("%d", CLIENT_COUNT);

    while (CLIENT_COUNT > 0) {
        message stop_client;
        msgrcv(serverQueue, &stop_client, TEXT_LEN, STOP, 0);
        stopHandler(&stop_client);
    }
    
    msgctl(serverQueue, IPC_RMID, NULL);
    exit(0);
}

void initHandler(message *msg){
    int id = atoi(msg -> text);

    struct Client *client = calloc(1, sizeof(client));

    client -> id = ID_COUNT;
    ID_COUNT++;
    client -> queueId = id;
    client -> connectedClientId = -1;

    clients[CLIENT_COUNT] = client;
    CLIENT_COUNT++;

    message reply;
    reply.type = INIT;
    sprintf(reply.text, "%d", client->id);
    msgsnd(id, &reply, TEXT_LEN, 0);
}

void listHandler(message *msg){
    int id = atoi(msg -> text);

    struct Client *client = getClient(id);

    message reply;
    reply.type = LIST;
    for (int i = 0; i < CLIENT_COUNT; i++) {
        sprintf(reply.text + strlen(reply.text), "%d: %d\n", clients[i]->id, clients[i] -> connectedClientId == -1);
    }
    
    msgsnd(client -> queueId, &reply, TEXT_LEN, 0);
}

void connectHandler(message *msg){
    int clientId = atoi(strtok(msg->text, " "));
    int secondId = atoi(strtok(NULL, " "));

    struct Client* first = getClient(clientId);
    struct Client* second = getClient(secondId);

    first -> connectedClientId = second -> id;
    second -> connectedClientId = first -> id;

    message reply;
    reply.type = CONNECT;

    sprintf(reply.text, "%d", first->queueId);
    msgsnd(second->queueId, &reply, TEXT_LEN, 0);
    sprintf(reply.text, "%d", second->queueId);
    msgsnd(first->queueId, &reply, TEXT_LEN, 0);
}   

void disconnectHandler(message *msg){
    int clientId = atoi(msg -> text);

    struct Client* first = getClient(clientId);
    struct Client* second = getClient(first -> connectedClientId);

    first -> connectedClientId = -1;
    second -> connectedClientId = -1;

    message reply;

    reply.type = DISCONNECT;
    msgsnd(second -> queueId, &reply, TEXT_LEN, 0);

}

void sigintHandler(int signum) {
    stopServer();
}

int main(){
    char* homePath = getpwuid(getuid())->pw_dir;
    printf("%d", CLIENT_COUNT);

    serverQueue = msgget(ftok(homePath, SERVER_KEY_ID), IPC_CREAT | 0666);

    signal(SIGINT, sigintHandler);

    while(1){
        message msg;
        msgrcv(serverQueue, &msg, TEXT_LEN, -TYPES_COUNT, 0);
        printf("%ld: %s\n", msg.type, msg.text);

        if (msg.type == STOP){
            stopHandler(&msg);
            continue;
        }

        if (msg.type == DISCONNECT){
            disconnectHandler(&msg);
            continue;
        }

        if (msg.type == LIST){
            listHandler(&msg);
            continue;
        }

        if (msg.type == INIT){
            initHandler(&msg);
            printf("INUTJE\n");
            continue;
        }

        if (msg.type == CONNECT){
            connectHandler(&msg);
            continue;
        }
    }

}