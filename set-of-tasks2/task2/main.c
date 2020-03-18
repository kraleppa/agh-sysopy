#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void printFile(char *path, struct stat statFile){
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

void max_depth(char *path, int depth){
    if (depth == 0){
        return;
    }

    if (path == NULL){
        return;
    }

    DIR *dir = opendir(path);

    if (dir == NULL){
        perror("directory does not exist");
        exit(-1);
    }

    struct dirent *file;

    while ((file = readdir(dir)) != NULL){
        char *newPath;
        newPath = concat("", concat(path, concat("/", file -> d_name)));
        struct stat statFile;
        lstat(newPath, &statFile);
        
        if (S_ISDIR(statFile.st_mode)){
            if (strcmp(file -> d_name, ".") != 0 && strcmp(file -> d_name, "..") != 0){
                max_depth(newPath, depth - 1);
            }
        }
        if (strcmp(file -> d_name, ".") != 0 && strcmp(file -> d_name, "..") != 0)
            printFile(newPath, statFile);
    }
    closedir(dir);
}

void mtime(char *path, char sign, int depth, time_t date){
    if (depth == 0){
        return;
    }

    if (path == NULL){
        return;
    }

    DIR *dir = opendir(path);

    if (dir == NULL){
        perror("directory does not exist");
        exit(-1);
    }

    struct dirent *file;

    while ((file = readdir(dir)) != NULL){
        char *newPath;
        newPath = concat("", concat(path, concat("/", file -> d_name)));
        struct stat statFile;
        lstat(newPath, &statFile);

        if (S_ISDIR(statFile.st_mode)){
            if (strcmp(file -> d_name, ".") != 0 && strcmp(file -> d_name, "..") != 0){
                mtime(newPath, sign, depth - 1, date);
            }
        }

        time_t modtime = statFile.st_mtime;
        int diff = difftime(date, modtime);

        if ((sign == '-' && diff <= 0) || (sign == '+' && diff >= 0)){
            if (strcmp(file -> d_name, ".") != 0 && strcmp(file -> d_name, "..") != 0)
            printFile(newPath, statFile);
        } 
    }
    closedir(dir);
}


int main(){
    // time_t rawtime;
    // struct tm *timeinfo;

    // time(&rawtime);
    // timeinfo = localtime(&rawtime);
    // timeinfo->tm_mday -= 1;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    tm.tm_mday -= 1;
    mtime("/home/krzysztof/Coding/Sysopy", '+', 1, mktime(&tm));
}