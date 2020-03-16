#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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

//return bigger string in lexicographical order.
// 0 -> equals, 1 -> string1 < string2, -1 -> string1 > string2
int compareStrings(char *string1, char *string2){
    int length = strlen(string1);
    for (int i = 0; i < length; i++){
        if (string1[i] > string2[i]){
            return -1;
        }
        if (string1[i] < string2[i]){
            return 1;
        }
    }
    return 0;
}

//return 'index'-th record from file.
char *getRecord(int fileDescriptor, int index, int lengthOfRecord){
    char * block = calloc(lengthOfRecord, sizeof(char));
    lseek(fileDescriptor, (lengthOfRecord + 1) * index, 0);
    read(fileDescriptor, block, lengthOfRecord);
    return block;
}

//save record on 'index'-th position in file
void saveRecord(int fileDescriptor, char *record, int index, int lengthOfRecord){
    lseek(fileDescriptor, (lengthOfRecord + 1) * index, 0);
    write(fileDescriptor, record, lengthOfRecord);
}

//swap record on index1 with record in index2 in file
void swapInFile(int fileDescriptor, int index1, int index2, int lengthOfRecord){
    char *tmp = getRecord(fileDescriptor, index1, lengthOfRecord);
    saveRecord(fileDescriptor, getRecord(fileDescriptor, index2, lengthOfRecord), index1, lengthOfRecord);
    saveRecord(fileDescriptor, tmp, index2, lengthOfRecord);
}

//partition with system commands
int partition(int fileDescriptor, int lengthOfRecord, int left, int right){
    char *pivot = getRecord(fileDescriptor, right, lengthOfRecord);
    int i = left - 1;
    for (int j = left; j < right; j++){
        char *record = getRecord(fileDescriptor, j, lengthOfRecord);
        if (compareStrings(record, pivot) >= 0){
            i = i + 1;
            swapInFile(fileDescriptor, i, j, lengthOfRecord);
        }
    }
    swapInFile(fileDescriptor, i + 1, right, lengthOfRecord);
    return i + 1;
}

//sorts file with system commands
void sort(int fileDescriptor, int lengthOfRecord, int left, int right){
    if (left < right){
        int q = partition(fileDescriptor, lengthOfRecord, left, right);
        sort(fileDescriptor, lengthOfRecord, left, q - 1);
        sort(fileDescriptor, lengthOfRecord, q + 1, right);
    }
}


int main(){
    generateWords(10, 4);
    int fileDescriptor = open("dane.txt", O_RDWR);
    system("cat dane.txt");
    // // 0 -> equals, 1 -> string1 < string2, -1 -> string1 > string2
    // printf("%d", compareStrings("AAAD", "BAAA"));
    sort(fileDescriptor, 4, 0, 9);
    close(fileDescriptor);
}