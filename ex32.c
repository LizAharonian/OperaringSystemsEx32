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
#define CONCAT 1000
#define UPDATE1 1
#define UPDATE2 2
#define UPDATE3 3
#define SPACE 32
#define NO_C_FILE "NO_C_FILE"
#define COMPILATION_ERROR "COMPILATION_ERROR"
#define TIMEOUT "TIMEOUT"
#define BAD_OUTPUT "BAD_OUTPUT"
#define SIMILAR_OUTPUT "SIMILAR_OUTPUT"
#define GREAT_JOB "GREAT_JOB"
#define STUSENTS_NUM 100

typedef struct students {
    char name[INPUT_SIZE];
    char cFilePath[INPUT_SIZE];
    char cfileDirPath[INPUT_SIZE];
    char grade[INPUT_SIZE];
    char reson[INPUT_SIZE];

} students;
//declarations
void handleFailure();
void readCMDFile(char* cmdFile,char directoryPath[INPUT_SIZE], char inputFilePath[INPUT_SIZE],char outputFilePath [INPUT_SIZE]);
void findTheCFile(char subDir[INPUT_SIZE],students* myStudents,int i,char cPath[INPUT_SIZE]);
void exploreSubDirs(char directoryPath[INPUT_SIZE],students* myStudents,int* i);
void gradeStudents(students* myStudents,int myStudentsSize, char inputFilePath[INPUT_SIZE], char outputFilePath[INPUT_SIZE]);
void runProgram(char inputFilePath[INPUT_SIZE],students* myStudents, int i,char outputFilePath[INPUT_SIZE]);
void writeResultsToCsv(students * myStudents, int i);

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
    //dynamic memory allocation for students arr
    students* myStudents=(students *)malloc(STUSENTS_NUM * sizeof(students));
    if (myStudents==NULL) {
        handleFailure();
    }
    int i =0;
    //explore students's dirs
    exploreSubDirs(directoryPath,myStudents,&i);
    //grade the students with appropiate reason
    gradeStudents(myStudents,i,inputFilePath,outputFilePath);
    //write the results to csv file
    writeResultsToCsv(myStudents,i);
    //free of memory allocation
    free(myStudents);
    return 0;
}

/**
 * writeResultsToCsv function.
 * @param myStudents - students arr
 * @param i - number of students
 */
void writeResultsToCsv(students * myStudents, int i){
    int resultsCsvFD;
    if((resultsCsvFD=open("results.csv",O_CREAT | O_TRUNC | O_WRONLY, 0644))==-1)
    {
        handleFailure();
    }
    int j;
    for (j=0;j<i;j++) {
        char concat[CONCAT] = {};
        strcpy(concat, myStudents[j].name);
        strcat(concat, ",");
        strcat(concat, myStudents[j].grade);
        strcat(concat, ",");
        strcat(concat, myStudents[j].reson);
        strcat(concat, "\n");
        if (write(resultsCsvFD, concat, CONCAT) < 0) {
            handleFailure();
        }
    }
    if (close(resultsCsvFD)==FAIL) {
        handleFailure();
    }

}

/**
 * callExecv function.
 * @param args - array for execvp function
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

/**
 * gradeStudents function.
 * @param myStudents - students arr
 * @param myStudentsSize - num of students
 * @param inputFilePath - input file
 * @param outputFilePath - output file
 */
void gradeStudents(students* myStudents,int myStudentsSize, char inputFilePath[INPUT_SIZE], char outputFilePath[INPUT_SIZE]) {
    int i;
    for (i = 0; i < myStudentsSize; i++) {
        //if the c file is not exist
        if (strcmp(myStudents[i].cFilePath, "\0") == 0) {
            strcpy(myStudents[i].grade, "0");
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
            args[1] ="-o";
            args[2]="temp.out";
            args[3] = cFileFullPath;
            args[4] = NULL;
            //call gcc
            callExecv(args);
            //check if temp.out exist
            if (isAOutExist() == FALSE) {
                strcpy(myStudents[i].grade, "0");
                strcpy(myStudents[i].reson, COMPILATION_ERROR);
            } else {
                //run the program
                runProgram(inputFilePath,myStudents,i,outputFilePath);
            }
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

        if (strcmp(dit->d_name, "temp.out") == 0) {
            isAoutExist = TRUE;
            break;
        }
    }
    return isAoutExist;
}

/**
 * run program function.
 * @param inputFilePath - unput file
 * @param myStudents - students arr
 * @param i - num of students
 * @param outputFilePath - output file
 */
void runProgram(char inputFilePath[INPUT_SIZE],students* myStudents, int i,char outputFilePath[INPUT_SIZE]) {
    //make args for temp.out
    char *args[INPUT_SIZE];
    char operation[INPUT_SIZE] = "./temp.out";
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
        if ((close(programInputFD)==FAIL)||(close(programOutputFD))==FAIL){
            handleFailure();
        }
    } else {   //father
        int value;
        sleep(5);
        pid_t returnPid = waitpid(pid, &value, WNOHANG);
        //if temp.out still running
        if (returnPid ==0) {
            strcpy(myStudents[i].grade, "0");
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
                if ( WIFEXITED(value) ) {
                    const int es = WEXITSTATUS(value);
                    switch (es) {
                        case 1:
                            strcpy(myStudents[i].grade, "60");
                            strcpy(myStudents[i].reson, BAD_OUTPUT);
                            break;
                        case 2:
                            strcpy(myStudents[i].grade, "80");
                            strcpy(myStudents[i].reson, SIMILAR_OUTPUT);
                            break;
                        case 3:
                            strcpy(myStudents[i].grade, "100");
                            strcpy(myStudents[i].reson, GREAT_JOB);
                            break;
                        default:
                            break;
                    }
                }
                if (unlink("temp.out") == FAIL||unlink("programOutput.txt")) {
                    handleFailure();
                }
            }
        }
    }
}

/**
 * exploreSubDirs function.
 * @param directoryPath - dir path
 * @param myStudents - my students arr
 * @param i - num of students
 */
void exploreSubDirs(char directoryPath[INPUT_SIZE],students* myStudents,int* i) {
    DIR* dip;
    struct dirent* dit;
    if((dip=opendir(directoryPath))==NULL){
        handleFailure();
    }
    int sizeOfStudentsArr = STUSENTS_NUM;

    while ((dit=readdir(dip))!=NULL) {
        if (dit->d_type == DT_DIR && strcmp(dit->d_name, ".") != 0 && strcmp(dit->d_name, "..") != 0) {
            //fill the students array
            strcpy(myStudents[*i].cFilePath, "\0");
            strcpy(myStudents[*i].name, "\0");
            strcpy(myStudents[*i].reson, "\0");
            strcpy(myStudents[*i].cfileDirPath, "\0");
            strcpy(myStudents[*i].name, dit->d_name);
            //concat the subDir path
            char subDir[INPUT_SIZE] = {};
            strcpy(subDir, directoryPath);
            int len = strlen(subDir);
            if (subDir[len] != '/') {
                strcat(subDir, "/");
            }
            strcat(subDir, dit->d_name);
            char cPath[INPUT_SIZE]={};
            //find the c file path
            findTheCFile(subDir, myStudents, *i, cPath);
            strcpy(myStudents[*i].cFilePath, cPath);
            (*i)++;
            //realloc in case is needed
            if ((*i)>=sizeOfStudentsArr) {
                sizeOfStudentsArr+=STUSENTS_NUM;
                myStudents = (students *)realloc(myStudents,sizeof(struct students)*sizeOfStudentsArr);
                if (myStudents==NULL){
                    handleFailure();
                }
            }
        }
    }

    if(closedir(dip)==FAIL){
        handleFailure();
    }
}

/**
 * findTheCFile function.
 * @param subDir - sub dir path
 * @param myStudents - students arr
 * @param i - num of students
 * @param cPath - c file path
 */
void findTheCFile(char subDir[INPUT_SIZE],students* myStudents,int i,char cPath[INPUT_SIZE]){
    DIR* dip;
    struct dirent* dit;
    if((dip=opendir(subDir))==NULL){
        handleFailure();
    }
    //read from the dir
    while ((dit=readdir(dip))!=NULL) {

        if (dit->d_type == DT_REG) {
            int len = strlen(dit->d_name);
            if (len >= 2 && dit->d_name[len - 1] == 'c' && dit->d_name[len - 2] == '.') {
                strcpy(myStudents[i].cfileDirPath, subDir);
                strcpy(cPath,dit->d_name);
                break;
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
            findTheCFile(subDirectory, myStudents, i,cPath);
        }

    }
}

/**
 * handleFailure function.
 * prints error and exits the program.
 */
void handleFailure() {
    write(STDERR, ERROR, ERROR_SIZE);
    exit(FAIL);
}

/**
 * readCMDFile function.
 * @param cmdFile - the configuration file
 * @param directoryPath - dir path
 * @param inputFilePath - input path
 * @param outputFilePath - output path
 */
void readCMDFile(char* cmdFile,char directoryPath[INPUT_SIZE], char inputFilePath[INPUT_SIZE],char outputFilePath [INPUT_SIZE]) {
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



