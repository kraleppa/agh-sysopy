#define _POSIX_C_SOURCE 200112L

#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "game.h"

typedef enum {
    START,
    WAIT_FOR_ENEMY,
    WAIT_FOR_MOVE,
    PROCESS_ENEMY_MOVE,
    MOVE,
    QUIT
} state_t;

state_t current_state = START;
char *cmd, *arg;
pthread_mutex_t reply_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reply_cond = PTHREAD_COND_INITIALIZER;
int server_socket;
int is_o;
char buff[MAX_MESSAGE_LENGTH + 1];
char* name;
board_t board;

void quit() {
    char buffer[MAX_MESSAGE_LENGTH + 1];
    sprintf(buffer, "quit: :%s", name);
    send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);

    exit(0);
}

void check_board_status() {
    int won = 0;
    board_object winner = get_winner(&board);
    if (winner != E) {
        if ((is_o && winner == O) || (!is_o && winner == X)) {
            printf("You won! :>");
        } else {
            printf("You lost! :<");
        }
        won = 1;
    }
    int drawn = 1;
    for (int i = 0; i < 9; i++) {
        if (board.objects[i] == E) {
            drawn = 0;
            break;
        }
    }
    if (drawn && !won) {
        puts("Draw");
    }
    if (won || drawn) {
        current_state = QUIT;
    }
}

void spliter(char* reply) {
    cmd = strtok(reply, ":");
    arg = strtok(NULL, ":");
}

void draw_board() {
    char symbols[3] = {' ', 'O', 'X'};
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            symbols[0] = y * 3 + x + 1 + '0';
            printf("|%c|", symbols[board.objects[y * 3 + x]]);
        }
        printf("\n---------");
    }
}

void g_loop(){
    if (current_state == START) {
        if (strcmp(arg, "name_taken") == 0) {
            printf("This nickname is already taken");
            exit(1);
        } else if (strcmp(arg, "no_enemy") == 0) {
            printf("waiting for the opponent");
            current_state = WAIT_FOR_ENEMY;
        } else {
            board = new_board();
            is_o = arg[0] == 'O';
            current_state = is_o ? MOVE : WAIT_FOR_MOVE;
        }
    } else if (current_state == WAIT_FOR_ENEMY) {
        pthread_mutex_lock(&reply_mutex);
        while (current_state != START && current_state != QUIT) {
            pthread_cond_wait(&reply_cond, &reply_mutex);
        }
        pthread_mutex_unlock(&reply_mutex);
        board = new_board();
        is_o = arg[0] == 'O';
        current_state = is_o ? MOVE : WAIT_FOR_MOVE;
    } else if (current_state == WAIT_FOR_MOVE) {
        printf("Waiting for the move");
        pthread_mutex_lock(&reply_mutex);
        while (current_state != PROCESS_ENEMY_MOVE && current_state != QUIT) {
            pthread_cond_wait(&reply_cond, &reply_mutex);
        }
        pthread_mutex_unlock(&reply_mutex);
    } else if (current_state == PROCESS_ENEMY_MOVE) {
        int move = atoi(arg);
        make_move(&board, move);
        check_board_status();
        if (current_state != QUIT) {
            current_state = MOVE;
        }
    } else if (current_state == MOVE) {
        draw_board();
        int move;
        do {
            printf("Enter next move (%c): ", is_o ? 'O' : 'X');
            scanf("%d", &move);
            move--;
        } while (!make_move(&board, move));
        draw_board();
        char buffer[MAX_MESSAGE_LENGTH + 1];
        sprintf(buffer, "move:%d:%s", move, name);
        send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        check_board_status();
        if (current_state != QUIT) {
            current_state = WAIT_FOR_MOVE;
        }
    } else if (current_state == QUIT) {
        quit();
    }
    g_loop();
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        perror("Syntax error");
        exit(1);
    }
    signal(SIGINT, quit);

    name = argv[1];
    char* type = argv[2];
    char* destination = argv[3];

    if (strcmp(type, "local") == 0) {
        server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
        struct sockaddr_un local_sockaddr;
        memset(&local_sockaddr, 0, sizeof(struct sockaddr_un));
        local_sockaddr.sun_family = AF_UNIX;
        strcpy(local_sockaddr.sun_path, destination);
        connect(server_socket, (struct sockaddr*)&local_sockaddr, sizeof(struct sockaddr_un));
    } else {
        struct addrinfo* info;
        struct addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        getaddrinfo("localhost", destination, &hints, &info);
        server_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        connect(server_socket, info->ai_addr, info->ai_addrlen);
        freeaddrinfo(info);
    }
    char buffer[MAX_MESSAGE_LENGTH + 1];
    sprintf(buffer, "add: :%s", name);
    send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
    int game_thread_running = 0;
    while (1) {
        recv(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        spliter(buffer);
        pthread_mutex_lock(&reply_mutex);
        if (strcmp(cmd, "add") == 0) {
            current_state = START;
            if (!game_thread_running) {
                pthread_t t;
                pthread_create(&t, NULL, (void* (*)(void*))g_loop, NULL);
                game_thread_running = 1;
            }
        } else if (strcmp(cmd, "move") == 0) {
            current_state = PROCESS_ENEMY_MOVE;
        } else if (strcmp(cmd, "quit") == 0) {
            current_state = QUIT;
            exit(0);
        } else if (strcmp(cmd, "ping") == 0) {
            sprintf(buffer, "pong: :%s", name);
            send(server_socket, buffer, MAX_MESSAGE_LENGTH, 0);
        }
        pthread_cond_signal(&reply_cond);
        pthread_mutex_unlock(&reply_mutex);
    }
}
