#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

void printFile(const char *path, struct stat statFile){
    char *fileType;

    if (S_ISREG(statFile.st_mode)){
        fileType = "file";
    } else if (S_ISDIR(statFile.st_mode)){
        fileType = "dir";
    } else if (S_ISCHR(statFile.st_mode)){
        fileType = "char";
    } else if (S_ISBLK(statFile.st_mode)){
        fileType = "block dev";
    } else if (S_ISFIFO(statFile.st_mode)){
        fileType = "fifo";
    } else if (S_ISLNK(statFile.st_mode)){
        fileType = "slink";
    } else if (S_ISSOCK(statFile.st_mode)){
        fileType = "socket";
    }

    time_t tm = statFile.st_mtime;
    struct tm ltm;
    localtime_r(&tm, &ltm);
    char modtime[80];
    strftime(modtime, sizeof(modtime), "%c", &ltm);

    time_t ta = statFile.st_atime;
    struct tm lta;
    localtime_r(&ta, &lta);
    char accestime[80];
    strftime(accestime, sizeof(accestime), "%c", &lta);
    
    printf("path: %s | links: %ld | file type: %s | file size: %ld | mod time: %s | acces time %s\n",
        path, statFile.st_nlink, fileType, statFile.st_size, modtime, accestime);
}

int maxdepth;
char *command;
char sign;
time_t date;

int fn(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    if (maxdepth != -1 && ftwbuf -> level > maxdepth){
        return 0;
    }

    if (strcmp(command, "-maxdepth") == 0)
    {
        printFile(fpath, *sb);
        return 0;
    }

    if (strcmp(command, "-mtime") == 0){
        time_t modtime = sb -> st_mtime;
        int diff = difftime(date, modtime);

        if ((sign == '-' && diff <= 0) || (sign == '+' && diff >= 0)){
            printFile(fpath, *sb);
        }
        return 0;
    }

    if (strcmp(command, "-atime") == 0){
        time_t atime = sb -> st_atime;
        int diff = difftime(date, atime);

        if ((sign == '-' && diff <= 0) || (sign == '+' && diff >= 0)){
            printFile(fpath, *sb);
        }
        return 0;
    }
    return 0;
}

int main(){
    maxdepth = 1;
    command = "-maxdepth";
    sign = '+';
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    tm.tm_mday -= 2;
    date = mktime(&tm);
    nftw("/home/krzysztof/Coding/Sysopy/set-of-tasks2/task2/directory", fn, FTW_D, FTW_PHYS);
}