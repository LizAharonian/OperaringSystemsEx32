/*
 * Liz Aharonian
 * 316584960
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

//define
#define Fail -1
#define TRUE 1
#define FALSE 0
#define BUFF_SIZE 1024
#define ERROR "Error in system call\n"
#define STDERR 2
#define ERROR_SIZE 21
#define INPUT_SIZE 200
#define UPDATE1 1
#define UPDATE2 2
#define UPDATE3 3
#define SPACE 32
#define tab 32


//declarations
void handleFailure();
int readCMDFile(char* cmdFile,char directoryPath[INPUT_SIZE], char inputFilePath[INPUT_SIZE],char outputFilePath [INPUT_SIZE]);
/**
 * main function.
 * runs the program.
 * @param argc - number of command line args.
 * @param argv - command line args.
 * @return
 */
int main(int argc, char **argv) {
    //validation of num of input args
    if (argc != 2) {
        handleFailure();
    }
    char directoryPath[INPUT_SIZE]={};
    char inputFilePath[INPUT_SIZE]={};
    char outputFilePath[INPUT_SIZE]={};
    readCMDFile(argv[1], directoryPath, inputFilePath, outputFilePath);
    printf("%s\n %s\n %s\n", directoryPath, inputFilePath, outputFilePath);

}

/**
 * handleFailure function.
 * prints error and exits the program.
 */
void handleFailure() {
    write(STDERR, ERROR, ERROR_SIZE);
    exit(Fail);
}


int readCMDFile(char* cmdFile,char directoryPath[INPUT_SIZE], char inputFilePath[INPUT_SIZE],char outputFilePath [INPUT_SIZE]) {
    //open files
    int file = open(cmdFile, O_RDONLY);
    if (file < 0) {
        handleFailure();
    }

    char buffer[BUFF_SIZE];
    int status;
    int count;
    while((count=read(file,buffer,sizeof(buffer)))>0)
    {
        int updateParam = UPDATE1;
        int i,j;
        i=0;
        j=0;
        for (;i<count;i++) {
            if (buffer[i]!='\n'&&buffer[i]!=SPACE&&buffer[i]!='\0') {
                switch (updateParam){
                    case UPDATE1:
                        directoryPath[j] = buffer[i];
                        break;
                    case UPDATE2:
                        inputFilePath[j] = buffer[i];
                        break;
                    case UPDATE3:
                        outputFilePath[j] = buffer[i];
                        break;

                }
                j++;

            } else{
                switch (updateParam){
                    case UPDATE1:
                        updateParam = UPDATE2;
                        j=0;
                        break;
                    case UPDATE2:
                        updateParam = UPDATE3;
                        j=0;
                        break;

                }

            }
        }
    }
    if (count < 0) {
        handleFailure();
    }

}



