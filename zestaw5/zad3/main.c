#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    mkfifo("pipe", S_IRUSR | S_IWUSR);

    if (fork() == 0){
        execlp("./producer", "./producer", "pipe", "2.txt", "10", NULL);
    }

    if (fork() == 0){
        execlp("./producer", "./producer", "pipe", "1.txt", "10", NULL);
    }

    if (fork() == 0){
        execlp("./producer", "./producer", "pipe", "3.txt", "10", NULL);
    }

    if (fork() == 0){
        execlp("./producer", "./producer", "pipe", "7.txt", "10", NULL);
    }

    if (fork() == 0){
        execlp("./producer", "./producer", "pipe", "o.txt", "10", NULL);
    }

    if (fork() == 0){
        execlp("./consumer", "./consumer", "pipe", "out.txt", "10", NULL);
    }

    
    
    while (wait(NULL) > 0);
}
