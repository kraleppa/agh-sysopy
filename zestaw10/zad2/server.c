#define _POSIX_C_SOURCE 200112L

#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "game.h"

typedef struct {
    char *name;
    int fd;
    struct sockaddr addr;
    int is_alive;
} client_t;

client_t* clients[MAX_PLAYERS] = {NULL};
int number_of_clients = 0;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int p_sockets(int l_socket, int n_socket) {
    struct pollfd pfds[2];
    pfds[0].fd = l_socket;
    pfds[0].events = POLLIN;
    pfds[1].fd = n_socket;
    pfds[1].events = POLLIN;

    poll(pfds, 2, -1);

    for (int i = 0; i < 2; i++) {
        if (pfds[i].revents & POLLIN) {
            return pfds[i].fd;
        }
    }

    return -1;
}

int get_player_by_name(char *name){
    for (int i = 0; i < MAX_PLAYERS; i++){
        if (clients[i] != NULL && strcmp(clients[i] -> name, name) == 0){
            return i;
        }
    }
    return -1;
}

int get_oponent(int i){
    if (i % 2 == 0){
        return i + 1;
    }
    return i - 1; 
}

int add_client(char* name, struct sockaddr addr, int fd){
    if (get_player_by_name(name) != -1){
        return -1;
    }

    int i = -1; 
    for (int j = 0; j < MAX_PLAYERS; j = j + 2) {
        if (clients[j] != NULL && clients[j + 1] == NULL) {
            i = j + 1;
            break;
        }
    }

     for (int j = 0; j < MAX_PLAYERS && j == -1; j++) {
        if (clients[j] == NULL) {
            i = j;
            break;
        }
    }

    if (i != -1) {
        client_t *new_client = calloc(1, sizeof(client_t));
        new_client -> name = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
        strcpy(new_client -> name, name);
        new_client->addr = addr;
        new_client -> fd = fd;
        new_client -> is_alive = 1;
        clients[i] = new_client;
        number_of_clients++;
    }

    return i;
}

void remove_client(char *name) {
    printf("removing client: %s\n", name);
    int c_i = get_player_by_name(name);
    if (c_i == -1) {
        return;
    }
    free(clients[c_i] -> name);
    free(clients[c_i]);
    clients[c_i] = NULL;
    number_of_clients--;
    int o_i = get_oponent(c_i);
    if (clients[o_i] != NULL) {
        puts("removing opponent");
        sendto(clients[o_i]->fd, "quit: ", MAX_MESSAGE_LENGTH, 0, &clients[o_i]->addr, sizeof(struct addrinfo));
        free(clients[o_i]->name);
        free(clients[o_i]);
        clients[o_i] = NULL;
        number_of_clients--;
    }
}

void p_loop() {
    printf("pinging");
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if ( !clients[i]->is_alive && clients[i] != NULL) {
            printf("removing ping: %s\n", clients[i] -> name);
            remove_client(clients[i] -> name);
        }
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clients[i] != NULL) {
            printf("sending");
            sendto(clients[i]->fd, "ping: ", MAX_MESSAGE_LENGTH, 0,
                   &clients[i]->addr, sizeof(struct addrinfo));
            clients[i] -> is_alive = 0;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    sleep(2);
    p_loop();
}

int get_l_socket(char *path) {
    int l_socket = socket(AF_UNIX, SOCK_DGRAM, 0);
    struct sockaddr_un local_sockaddr;
    memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
    local_sockaddr.sun_family = AF_UNIX;
    strcpy(local_sockaddr.sun_path, path);
    unlink(path);
    bind(l_socket, (struct sockaddr*)&local_sockaddr, sizeof(struct sockaddr_un));
    return l_socket;
}

int get_n_socket(char *port) {
    struct addrinfo* info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, port, &hints, &info);
    int n_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    bind(n_socket, info->ai_addr, info->ai_addrlen);
    freeaddrinfo(info);
    return n_socket;
}

int main(int argc, char* argv[]){
    if (argc != 3) {
        perror("Wrong syntax");
        exit(1);
    }
    srand(time(NULL));
    char* port = argv[1];
    char* s_path = argv[2];
    int l_socket = get_l_socket(s_path);
    int n_socket = get_n_socket(port);

    pthread_t thread;
    pthread_create(&thread, NULL, (void* (*)(void*))p_loop, NULL);
    while (1) {
        int socket_fd = p_sockets(l_socket, n_socket);

        char buffer[MAX_MESSAGE_LENGTH + 1];
        struct sockaddr from_addr;
        socklen_t from_length = sizeof(struct sockaddr);
        recvfrom(socket_fd, buffer, MAX_MESSAGE_LENGTH, 0, &from_addr,
                 &from_length);
        puts(buffer);

        char* cmd = strtok(buffer, ":");
        char* arg = strtok(NULL, ":");
        char* nickname = strtok(NULL, ":");

        pthread_mutex_lock(&clients_mutex);
        if (strcmp(cmd, "add") == 0) {
            int index = add_client(nickname, from_addr, socket_fd);

            if (index == -1) {
                sendto(socket_fd, "add:name_taken", MAX_MESSAGE_LENGTH, 0,
                       (struct sockaddr*)&from_addr, sizeof(struct addrinfo));
            } else if (index % 2 == 0) {
                sendto(socket_fd, "add:no_enemy", MAX_MESSAGE_LENGTH, 0,
                       (struct sockaddr*)&from_addr, sizeof(struct addrinfo));
            } else {
                int waiting_client_goes_first = rand() % 2;
                int first_player_index = index - waiting_client_goes_first;
                int second_player_index = get_oponent(first_player_index);

                sendto(clients[first_player_index]->fd, "add:O",
                       MAX_MESSAGE_LENGTH, 0,
                       &clients[first_player_index]->addr,
                       sizeof(struct addrinfo));
                sendto(clients[second_player_index]->fd, "add:X",
                       MAX_MESSAGE_LENGTH, 0,
                       &clients[second_player_index]->addr,
                       sizeof(struct addrinfo));
            }
        }
        if (strcmp(cmd, "move") == 0) {
            int move = atoi(arg);
            int player = get_player_by_name(nickname);

            sprintf(buffer, "move:%d", move);
            sendto(clients[get_oponent(player)]->fd, buffer,
                   MAX_MESSAGE_LENGTH, 0, &clients[get_oponent(player)]->addr,
                   sizeof(struct addrinfo));
        }
        if (strcmp(cmd, "quit") == 0) {
            remove_client(nickname);
        }
        if (strcmp(cmd, "pong") == 0) {
            int player = get_player_by_name(nickname);
            if (player != -1) {
                clients[player]->is_alive = 1;
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
}