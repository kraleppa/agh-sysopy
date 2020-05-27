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
    int is_alive;
} client_t;

client_t* clients[MAX_PLAYERS] = {NULL};
int number_of_clients = 0;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int p_sockets(int l_socket, int n_socket) {
    struct pollfd *pfds = calloc(number_of_clients + 2, sizeof(struct pollfd));
    pfds[0].fd = l_socket;
    pfds[0].events = POLLIN;
    pfds[1].fd = n_socket;
    pfds[1].events = POLLIN;
    pthread_mutex_lock(&clients_mutex);

    for(int i = 0; i < number_of_clients; i++){
        pfds[i + 2].fd = clients[i] -> fd;
        pfds[i + 2].events = POLLIN;
    }
    pthread_mutex_unlock(&clients_mutex);
    poll(pfds, number_of_clients + 2, -1);

    int return_value;
    for (int i = 0; i < number_of_clients + 2; i++){
        if (pfds[i].revents & POLLIN) {
            return_value = pfds[i].fd;
            break;
        }
    }

    if (return_value == l_socket || return_value == n_socket){
        return_value = accept(return_value, NULL, NULL);
    }

    free(pfds);
    return return_value;
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

int add_client(char *name, int fd){
    if (get_player_by_name(name) != -1){
        return -1;
    }

    int i = -1; 
    for (int i = 0; i < MAX_PLAYERS; i = i + 2) {
        if (clients[i] != NULL && clients[i + 1] == NULL) {
            i = i + 1;
            break;
        }
    }

     for (int i = 0; i < MAX_PLAYERS && i == -1; i++) {
        if (clients[i] == NULL) {
            i = i;
            break;
        }
    }

    if (i != -1) {
        client_t *new_client = calloc(1, sizeof(client_t));
        new_client -> name = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
        strcpy(new_client -> name, name);
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
        printf("removing opponent");
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
            send(clients[i]->fd, "ping: ", MAX_MESSAGE_LENGTH, 0);
            clients[i] -> is_alive = 0;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    sleep(2);
    p_loop();
}

int get_l_socket(char *path) {
    int l_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un l_sockaddr;
    memset(&l_sockaddr, 0, sizeof(struct sockaddr_un));
    l_sockaddr.sun_family = AF_UNIX;
    strcpy(l_sockaddr.sun_path, path);
    unlink(path);
    bind(l_socket, (struct sockaddr*)&l_sockaddr, sizeof(struct sockaddr_un));
    listen(l_socket, MAX_BACKLOG);

    return l_socket;
}

int get_n_socket(char *port) {
    struct addrinfo* info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, port, &hints, &info);
    int n_socket = socket(info -> ai_family, info -> ai_socktype, info -> ai_protocol);
    bind(n_socket, info -> ai_addr, info -> ai_addrlen);
    listen(n_socket, MAX_BACKLOG);
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
        int client_fd = p_sockets(l_socket, n_socket);
        char buff[MAX_MESSAGE_LENGTH + 1];
        recv(client_fd, buff, MAX_MESSAGE_LENGTH, 0);
        printf("%s", buff);
        char *cmd = strtok(buff, ":");
        char *arg = strtok(NULL, ":");
        char *nickname = strtok(NULL, ":");
        pthread_mutex_lock(&clients_mutex);
        if (strcmp(cmd, "add") == 0) {
            int i = add_client(nickname, client_fd);
            if (i == -1) {
                send(client_fd, "add:name_taken", MAX_MESSAGE_LENGTH, 0);
                close(client_fd);
            } else if (i % 2 == 0) {
                send(client_fd, "add:no_enemy", MAX_MESSAGE_LENGTH, 0);
            } else {
                int waiting_client_goes_first = rand() % 2;
                int first_player_i = i - waiting_client_goes_first;
                int second_player_i = get_oponent(first_player_i);

                send(clients[first_player_i]->fd, "add:O",
                     MAX_MESSAGE_LENGTH, 0);
                send(clients[second_player_i]->fd, "add:X",
                     MAX_MESSAGE_LENGTH, 0);
            }
        }
        if (strcmp(cmd, "move") == 0) {
            int move = atoi(arg);
            int player = get_player_by_name(nickname);

            sprintf(buff, "move:%d", move);
            send(clients[get_oponent(player)]->fd, buff, MAX_MESSAGE_LENGTH, 0);
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