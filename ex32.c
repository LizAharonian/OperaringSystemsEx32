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
#include <wait.h>

//define
#define FAIL -1
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
#define TAB 32
#define NO_C_FILE "NO_C_FILE"
#define COMPILATION_ERROR "COMPILATION_ERROR"
#define TIMEOUT "TIMEOUT"
#define BAD_OUTPUT "BAD_OUTPUT"
#define SIMILAR_OUTPUT "SIMILAR_OUTPUT"
#define GREAT_JOB "GREAT_JOB"




//declarations
void handleFailure();
int readCMDFile(char* cmdFile,char directoryPath[INPUT_SIZE], char inputFilePath[INPUT_SIZE],char outputFilePath [INPUT_SIZE]);
typedef struct students {
    char name[INPUT_SIZE];
    char cFilePath[INPUT_SIZE];
    char cfileDirPath[INPUT_SIZE];
    int grade;
    char reson[INPUT_SIZE];

} students;
char * findTheCFile(char subDir[INPUT_SIZE],students* myStudents,int i);
void exploreSubDirs(char directoryPath[INPUT_SIZE],students* myStudents,int* i);
void gradeStudents(students* myStudents,int myStudentsSize, char inputFilePath[INPUT_SIZE], char outputFilePath[INPUT_SIZE]);
void runProgram(char inputFilePath[INPUT_SIZE],students* myStudents, int i,char outputFilePath[INPUT_SIZE]);

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
    int i =0;

    exploreSubDirs(directoryPath,myStudents,&i);
    gradeStudents(myStudents,i,inputFilePath,outputFilePath);

    //printf("liz");
    return 0;
}

/**
 * callExecv function.
 * @param args - array for execvp function
 * @param isBackground - tells if parent need to wait to son
 * @return
 */
void callExecv(char **args) {

    int stat, retCode;
    pid_t pid;
    pid = fork();
    if (pid == 0) {  // son
        retCode = execvp(args[0], &args[0]);
        if (retCode == FAIL) {
            handleFailure();
        }
    } else {   //father
        waitpid(pid, NULL, WCONTINUED);

    }
}

void gradeStudents(students* myStudents,int myStudentsSize, char inputFilePath[INPUT_SIZE], char outputFilePath[INPUT_SIZE]) {
    int i;
    for (i = 0; i < myStudentsSize; i++) {
        //if the c file is not exist
        if (strcmp(myStudents[i].cFilePath, "\0") == 0) {
            myStudents[i].grade = 0;
            strcpy(myStudents[i].reson, NO_C_FILE);
        } else {
            //make args for compilation
            char *args[INPUT_SIZE];
            char operation[INPUT_SIZE] = "gcc";
            args[0] = operation;
            char cFileFullPath[INPUT_SIZE] = {};
            strcpy(cFileFullPath, myStudents[i].cfileDirPath);
            strcat(cFileFullPath, "/");
            strcat(cFileFullPath, myStudents[i].cFilePath);
            args[1] = cFileFullPath;
            args[2] = NULL;
            //call gcc
            callExecv(args);
            //check if a.out exist
            if (isAOutExist() == FALSE) {
                myStudents[i].grade = 0;
                strcpy(myStudents[i].reson, COMPILATION_ERROR);
            } else {
                //run the program
                runProgram(inputFilePath,myStudents,i,outputFilePath);
            }


            //todo: delete all created files
           /* if (unlink("a.out") == FAIL) {
                handleFailure();
            }*/



        }
    }

}
int isAOutExist() {
    int isAoutExist = FALSE;
    DIR *dip;
    struct dirent *dit;
    char cwd[BUFF_SIZE];
    getcwd(cwd, sizeof(cwd));
    if ((dip = opendir(cwd)) == NULL) {
        handleFailure();
    }

    while ((dit = readdir(dip)) != NULL) {

        if (strcmp(dit->d_name, "a.out") == 0) {
            isAoutExist = TRUE;
            break;
        }
    }
    return isAoutExist;
}

void runProgram(char inputFilePath[INPUT_SIZE],students* myStudents, int i,char outputFilePath[INPUT_SIZE]) {
    //make args for a.out
    char *args[INPUT_SIZE];
    char operation[INPUT_SIZE] = "./a.out";
    args[0] = operation;
    args[1] = NULL;
    int stat, retCode;
    pid_t pid;
    pid = fork();
     if (pid == 0) {  // son
         int programOutputFD;
         if ((programOutputFD = open("programOutput.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0) {
             handleFailure();
         }
         int programInputFD;
         if ((programInputFD = open(inputFilePath, O_RDONLY)) < 0) {
             handleFailure();
         }

         retCode = dup2(programInputFD, 0);
         if (retCode == FAIL) {
             handleFailure();
         }
         retCode = dup2(programOutputFD, 1);
         if (retCode == FAIL) {
             handleFailure();
         }
         retCode = execvp(args[0], &args[0]);
         if (retCode == FAIL) {
             handleFailure();
         }
         close(programInputFD);
         close(programOutputFD);
     } else {   //father
         int value;
         sleep(5);
         pid_t returnPid = waitpid(pid, &value, WNOHANG);
         //if a.out still running
         if (returnPid ==0) {
             myStudents[i].grade=0;
             strcpy(myStudents[i].reson,TIMEOUT);
         }else { //start comparisson
             //make args for comp.out
             char *args[INPUT_SIZE];
             char operation[INPUT_SIZE] = "./comp.out";
             args[0] = operation;
             args[1] = "programOutput.txt";
             args[2] = outputFilePath;
             args[3] = NULL;
             pid_t pid;
             pid = fork();
             if (pid == 0) {  // son

                 retCode = execvp(args[0], &args[0]);
                 if (retCode == FAIL) {
                     handleFailure();
                 }
             } else {//father
                 waitpid(pid, &value, 0);


                 switch (value) {
                     case 1:
                         myStudents[i].grade = 60;
                         strcpy(myStudents[i].reson, BAD_OUTPUT);
                         break;
                     case 2:
                         myStudents[i].grade = 80;
                         strcpy(myStudents[i].reson, SIMILAR_OUTPUT);
                         break;
                     case 3:
                         myStudents[i].grade = 100;
                         strcpy(myStudents[i].reson, GREAT_JOB);
                         break;
                     default:
                         break;
                 }


             }

         }
         /*else if (value!=0) {//running failed
             myStudents[i].grade=0;
             strcpy(myStudents[i].reson,TIMEOUT);
         }*/

     }

    //todo:make comparison between the output and the expected output
    //todo: close fd's and dirs!!

}
void exploreSubDirs(char directoryPath[INPUT_SIZE],students* myStudents,int* i) {
    DIR* dip;
    struct dirent* dit;
    if((dip=opendir(directoryPath))==NULL){
        handleFailure();
    }

    while ((dit=readdir(dip))!=NULL) {
        if(dit->d_type==DT_DIR &&strcmp(dit->d_name,".")!=0&&strcmp(dit->d_name,"..")!=0) {
            //debug only
            printf("%s\n", dit->d_name);

            //fill the students array
            strcpy(myStudents[*i].cFilePath,"\0");
            strcpy(myStudents[*i].name,"\0");
            strcpy(myStudents[*i].reson,"\0");
            strcpy(myStudents[*i].cfileDirPath,"\0");
            strcpy(myStudents[*i].name,dit->d_name);
            //concat the subDir path
            char subDir[INPUT_SIZE]={};
            strcpy(subDir,directoryPath);
            int len = strlen(subDir);
            if (subDir[len]!='/') {
                strcat(subDir,"/");
            }
            strcat(subDir,dit->d_name);

            strcpy(myStudents[*i].cFilePath,findTheCFile(subDir,myStudents,*i));
            (*i)++;

        }
    }

    if(closedir(dip)==FAIL){
        handleFailure();
    }
    //return myStudents;
}

char * findTheCFile(char subDir[INPUT_SIZE],students* myStudents,int i){
    DIR* dip;
    struct dirent* dit;
    if((dip=opendir(subDir))==NULL){
        handleFailure();
    }

    while ((dit=readdir(dip))!=NULL) {

        if (dit->d_type == DT_REG) {
            int len = strlen(dit->d_name);
            printf("%s\n", dit->d_name);
            if (len >= 2 && dit->d_name[len - 1] == 'c' && dit->d_name[len - 2] == '.') {
                printf("----%s\n", dit->d_name);
                strcpy(myStudents[i].cfileDirPath, subDir);
                //concat the subDir path
                /*char subDirectory[INPUT_SIZE]={};
                strcpy(subDirectory,subDir);

                int len = strlen(subDirectory);
                if (subDirectory[len]!='/') {
                    strcat(subDirectory,"/");
                }
                strcat(subDirectory,dit->d_name);
                return subDirectory;*/
                return dit->d_name;
            }

        } else if (dit->d_type == DT_DIR && strcmp(dit->d_name, ".") != 0 && strcmp(dit->d_name, "..") != 0) {
            //concat the subDir path
            char subDirectory[INPUT_SIZE]={};
            strcpy(subDirectory,subDir);

            int len = strlen(subDirectory);
            if (subDirectory[len]!='/') {
                strcat(subDirectory,"/");
            }
            strcat(subDirectory,dit->d_name);
            return findTheCFile(subDirectory, myStudents, i);
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
    exit(FAIL);
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



