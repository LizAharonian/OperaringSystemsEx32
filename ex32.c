/*
 * Liz Aharonian
 * 316584960
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <memory.h>

//define
#define Fail -1
#define TRUE 1
#define FALSE 0
#define BUFF_SIZE 1024
#define ERROR "Error in system call\n"
#define STDERR 2
#define ERROR_SIZE 21
#define INPUT_SIZE 300
#define UPDATE1 1
#define UPDATE2 2
#define UPDATE3 3
#define SPACE 32
#define tab 32


//declarations
void handleFailure();
int readCMDFile(char* cmdFile,char directoryPath[INPUT_SIZE], char inputFilePath[INPUT_SIZE],char outputFilePath [INPUT_SIZE]);
typedef struct students {
    char name[INPUT_SIZE];
    char cFilePath[INPUT_SIZE];
    int grade;

} students;
char * findTheCFile(char subDir[INPUT_SIZE]);
void exploreSubDirs(char directoryPath[INPUT_SIZE],students* myStudents);


void gradeStudents(students myStudents[300]);

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
    char directoryPath[INPUT_SIZE] = {};
    char inputFilePath[INPUT_SIZE] = {};
    char outputFilePath[INPUT_SIZE] = {};
    readCMDFile(argv[1], directoryPath, inputFilePath, outputFilePath);
    printf("cmd file text:\n%s\n%s\n%s\n", directoryPath, inputFilePath, outputFilePath);
    //todo: allocate dynamic memory
    students myStudents[INPUT_SIZE];

    exploreSubDirs(directoryPath,myStudents);
    gradeStudents(myStudents);

    printf("liz");
    return 0;
}

void gradeStudents(students* myStudents) {

   // while ()

}

void exploreSubDirs(char directoryPath[INPUT_SIZE],students* myStudents) {
    DIR* dip;
    struct dirent* dit;
    if((dip=opendir(directoryPath))==NULL){
        handleFailure();
    }

    int i=0;

    while ((dit=readdir(dip))!=NULL) {
        if(dit->d_type==DT_DIR &&strcmp(dit->d_name,".")!=0&&strcmp(dit->d_name,"..")!=0) {
            //debug only
            printf("%s\n", dit->d_name);

            //fill the students array
            strcpy(myStudents[i].cFilePath,"\0");
            strcpy(myStudents[i].name,"\0");
            strcpy(myStudents[i].name,dit->d_name);
            //concat the subDir path
            char subDir[INPUT_SIZE]={};
            strcpy(subDir,directoryPath);
            int len = strlen(subDir);
            if (subDir[len]!='/') {
                strcat(subDir,"/");
            }
            strcat(subDir,dit->d_name);
            strcpy(myStudents[i].cFilePath,findTheCFile(subDir));
            i++;

        }
    }

    if(closedir(dip)==Fail){
        handleFailure();
    }
    return myStudents;
}

char * findTheCFile(char subDir[INPUT_SIZE]){
    DIR* dip;
    struct dirent* dit;
    if((dip=opendir(subDir))==NULL){
        handleFailure();
    }

    while ((dit=readdir(dip))!=NULL) {

        if (dit->d_type==DT_REG){
            int len = strlen(dit->d_name);
            printf("%s\n",dit->d_name);
            if(len>=2&&dit->d_name[len-1]=='c'&&dit->d_name[len-2]=='.'){
                printf("----%s\n",dit->d_name);

                return dit->d_name;
            }

        }
    }
    //in case c file is not exist
    return "\0";
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



