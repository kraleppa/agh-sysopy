#include <mqueue.h>
#include <stdlib.h>

#define TEXT_LEN 128
#define FILENAME_LEN 16
#define SERVER_KEY_ID 2137
#define MAX_CLIENTS 10

#define STOP_SERVER 1L
#define STOP 2L
#define DISCONNECT 3L
#define LIST 4L
#define INIT 5L
#define CONNECT 6L
#define SEND 7L
#define TYPES_COUNT 7L

typedef struct {
    int id;
    int queueId;
    char* queueFilename;
    int connectedClientId;
} Client;

typedef struct {
    long type;
    char text[TEXT_LEN];
} message;

