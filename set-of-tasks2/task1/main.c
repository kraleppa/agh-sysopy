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


//----------------------------------------------------------------------------------------------------
//system sorting

//return 'index'-th record from file.
char *getRecordSys(int fileDescriptor, int index, int lengthOfRecord){
    char *record = calloc(lengthOfRecord, sizeof(char));
    lseek(fileDescriptor, (lengthOfRecord + 1) * index, 0);
    read(fileDescriptor, record, lengthOfRecord);
    return record;
}

//save record on 'index'-th position in file
void saveRecordSys(int fileDescriptor, char *record, int index, int lengthOfRecord){
    lseek(fileDescriptor, (lengthOfRecord + 1) * index, 0);
    write(fileDescriptor, record, lengthOfRecord);
}

//swap record on index1 with record in index2 in file
void swapInFileSys(int fileDescriptor, int index1, int index2, int lengthOfRecord){
    char *tmp = getRecordSys(fileDescriptor, index1, lengthOfRecord);
    saveRecordSys(fileDescriptor, getRecordSys(fileDescriptor, index2, lengthOfRecord), index1, lengthOfRecord);
    saveRecordSys(fileDescriptor, tmp, index2, lengthOfRecord);
}

int partitionSys(int fileDescriptor, int lengthOfRecord, int left, int right){
    char *pivot = getRecordSys(fileDescriptor, right, lengthOfRecord);
    int i = left - 1;
    for (int j = left; j < right; j++){
        char *record = getRecordSys(fileDescriptor, j, lengthOfRecord);
        if (compareStrings(record, pivot) >= 0){
            i = i + 1;
            swapInFileSys(fileDescriptor, i, j, lengthOfRecord);
        }
    }
    swapInFileSys(fileDescriptor, i + 1, right, lengthOfRecord);
    return i + 1;
}

void quickSortSys(int fileDescriptor, int lengthOfRecord, int left, int right){
    if (left < right){
        int q = partitionSys(fileDescriptor, lengthOfRecord, left, right);
        quickSortSys(fileDescriptor, lengthOfRecord, left, q - 1);
        quickSortSys(fileDescriptor, lengthOfRecord, q + 1, right);
    }
}

void sortSys(char *fileName, int lengthOfRecord, int numberOfRecords){
    int fileDescriptor = open(fileName, O_RDWR);
    if (fileDescriptor < 0){
        perror("file does not exist");
    }

    quickSortSys(fileDescriptor, lengthOfRecord, 0, numberOfRecords - 1);
    close(fileDescriptor);
}

//system copy
void copySys(char *fileName1, char *fileName2, int lengthOfRecord, int numberOfRecords){
    int fileDescriptor1 = open(fileName1, O_RDONLY);
    if (fileDescriptor1 < 0){
        perror("file does not exist");
    }

    int fileDescriptor2 = open(fileName2, O_WRONLY);
    if (fileDescriptor2 < 0){
        system("touch copy.txt");
        fileDescriptor2 = open("copy.txt", O_WRONLY);
    }

    char buffer[lengthOfRecord + 1];
    for (int i = 0; i < numberOfRecords; i++){
        read(fileDescriptor1, buffer, lengthOfRecord + 1);
        write(fileDescriptor2, buffer, lengthOfRecord + 1);
    }

    close(fileDescriptor1);
    close(fileDescriptor2);
}

//----------------------------------------------------------------------------------------------------
//library sorting

char *getRecordLib(FILE *file, int index, int lengthOfRecord){
    char *record = calloc(lengthOfRecord, sizeof(char));
    fseek(file, (lengthOfRecord + 1) * index, 0);
    fread(record, sizeof(char), lengthOfRecord, file);
    return record;
}

void saveRecordLib(FILE *file, char *record, int index, int lengthOfRecord){
    fseek(file, (lengthOfRecord + 1) * index, 0);
    fwrite(record, sizeof(char), lengthOfRecord, file);
}

void swapInFileLib(FILE *file, int index1, int index2, int lengthOfRecord){
    char *tmp = getRecordLib(file, index1, lengthOfRecord);
    saveRecordLib(file, getRecordLib(file, index2, lengthOfRecord), index1, lengthOfRecord);
    saveRecordLib(file, tmp, index2, lengthOfRecord);
}

int partitionLib(FILE *file, int lengthOfRecord, int left, int right){
    char *pivot = getRecordLib(file, right, lengthOfRecord);
    int i = left - 1;
    for (int j = left; j < right; j++){
        char *record = getRecordLib(file, j, lengthOfRecord);
        if (compareStrings(record, pivot) >= 0){
            i = i + 1;
            swapInFileLib(file, i, j, lengthOfRecord);
        }
    }
    swapInFileLib(file, i + 1, right, lengthOfRecord);
    return i + 1;
}

void quickSortLib(FILE *file, int lengthOfRecord, int left, int right){
    if (left < right){
        int q = partitionLib(file, lengthOfRecord, left, right);
        quickSortLib(file, lengthOfRecord, left, q - 1);
        quickSortLib(file, lengthOfRecord, q + 1, right);
    }
}

void sortLib(char *fileName, int lengthOfRecord, int numberOfRecord){
    FILE *file = fopen(fileName, "r+");
    if (file == NULL){
        perror("file does not exist");
    }

    quickSortLib(file, lengthOfRecord, 0, numberOfRecord - 1);
    fclose(file);
}

//library copy
void copyLib(char *fileName1, char *fileName2, int lengthOfRecord, int numberOfRecords){
    FILE *file1 = fopen(fileName1, "r");
    if (file1 == NULL){
        perror("file does not exist");
    }

    FILE *file2 = fopen(fileName2, "w");
    if (file2 == NULL){
        system("touch copy.txt");
        file2 = fopen("copy.txt", "w");
    }

    char buffer[lengthOfRecord + 1];
    for (int i = 0; i < numberOfRecords; i++){
        fread(buffer, sizeof(char), lengthOfRecord + 1, file1);
        fwrite(buffer, sizeof(char), lengthOfRecord + 1, file2);
    }

    fclose(file1);
    fclose(file2);
}



int main(){
    int lengthOfRecords = 10;
    generateWords(10, lengthOfRecords);
    copyLib("dane.txt", "copy.txt", lengthOfRecords, 10);
}