#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//connect two strings
char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

//generate random word
char *generateWord(int length){
    char *word = calloc(length, sizeof(char));
    for (int i = 0; i < length; i++){
        word[i] = 65 + rand() % 26;
    }
    return word;
}

//generate 'number' words and save them to dane.txt
void generateWords(int number, int length){
    system("rm -f dane.txt");
    system("touch dane.txt");

    for (int i = 0; i < number; i++){
        char *word = generateWord(length);
        char *command = concat("echo ", concat(word, " >> dane.txt"));
        system(command);
    }

}

int main(){
    generateWords(10, 4);
}