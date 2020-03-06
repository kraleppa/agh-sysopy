#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>

//connects two strings
char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

//compares two files and saves the result to tmp.txt
void compareTwoFiles(char *fileName1, char *fileName2){
    char *command = concat("diff ", concat(fileName1, concat(" ", concat(fileName2, " > tmp.txt"))));
    system(command);
}

int getNumberOfOperations(){
    FILE *filePoiner = fopen("tmp.txt", "r");
    if (filePoiner == NULL){
        exit(EXIT_FAILURE);
    }

    char buffer[255];
    int numberOfOperations = 0;

    while (fgets(buffer, 255, filePoiner)){
        if (isdigit(buffer[0])){
            numberOfOperations++;
        }  
    }
    return numberOfOperations;
}

void initializeBlockOfEditingOperations(){
    FILE *filePoiner = fopen("tmp.txt", "r");
    char buffer[255];
    if (filePoiner == NULL){
        exit(EXIT_FAILURE);
    }
    int numberOfOperations = getNumberOfOperations();
    printf("AAAAAa");
    char **block = calloc(numberOfOperations, sizeof(char*));
    
    int i = 0;
    while (fgets(buffer, 255, filePoiner)){
        if (isdigit(buffer[0])){
            block[i] = "eluwina";
            i++;
        }  
    }
}


int main(){
    initializeBlockOfEditingOperations();
}