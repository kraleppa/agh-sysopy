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
        printf("%s\n", newPath);
    }
    closedir(dir);
}

int main(){
    max_depth("directory/lvl1", 3);
}