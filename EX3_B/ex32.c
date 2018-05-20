

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>


#define SIZE_FILE 480
#define SIZE_LINE 160
#define SIZE_DESC 60
#define STD_IN 0
#define STD_OUT 1
#define STD_ERR 2


#define NO_C_FILE "N0_C_FILE"
#define COMPILATION_ERROR "COMPILATION_ERROR"
#define TIMEOUT "TIMEOUT"
#define BAD_OUTPUT "BAD_OUTPUT"
#define SIMILAR_OUTPUT "SIMILAR_OUTPUT"
#define GREAT_JOB "GREAT_JOB"



typedef struct StudentGrade {
    char*  name;
    char  description[SIZE_DESC];
    int   grade;
} StudentGrade;

/**
 * this method read configurtion file and put each line in char**.
 * @param configFile - path to config file.
 * @param line - put each line in char** line.
 * @param fs - file descriptor
 */
void readConfigureFile(char* configFile,char** line, int fs);
/**
 * this method search C file in all folder.
 * @param studentName - name of student.
 * @param dirLocation - path to current location.
 * @param inputLocation - the second line of config file.
 * @param outputLocation  - the third line of config file.
 * @param isFound - indicate if c file was found.
 * @return return struct that contain grade;name and description.
 */
StudentGrade searchCFile(char* studentName, char* dirLocation, char* inputLocation, char* outputLocation, int* isFound);
/**
 * this method run the c file
 * @param studentName - name of student.
 * @param inputLocation - the second line of config file.
 * @param outputLocation  - the third line of config file.
 * @param fileName compile file name.
 * @return return struct that contain grade;name and description.
 */
StudentGrade runProgram(char* studentName, char* inputLocation, char* outputLocation, char* fileName);
/**
 * this method compile a c file
 * @param studentName- name of student.
 * @param path path to current location.
 * @param inputLocation - the second line of config file.
 * @param outputLocation - the third line of config file.
 * @return return struct that contain grade;name and description.
 */
StudentGrade compileFile(char* studentName, char* path, char* inputLocation, char* outputLocation);

/**
 * write to stderr 'error in system call'
 */
void errorSystemCall();
/**
 * this method check if the path lead to directory.
 * @param path
 * @return 1 if it is directory; 0 if not
 */
int isDirectory(const char* path);
/**
 * this method use "comp.out" to cpmpare between 2 files
 * @param file - the output of c file.
 * @param output - the acceptable output
 * @return return 1 if different; 2 if  similar; 3 if identical
 */
int CompareFiles(char* file, char* output);
/**
 * this method append to path the name of folder and save it in current.
 * @param current - save to it.
 * @param line - path
 * @param name - name of folder
 */
void buildPath(char* current,const char* line,char* name);
/**
 * this method open file
 * @param filename - path
 * @return file descriptor.
 */
int openFile(const char *filename);


int main(int argc, char *argv[]) {
    char buffer[SIZE_FILE];
    char *line[SIZE_FILE];
    char current[SIZE_LINE];
    int fdResults;
    char s[SIZE_FILE];
    int copySTDOUT;
    int isFound = 0;
    int fs;
    DIR *pDir;
    struct dirent *pDirent;


    if (argc != 2) {
        printf("The program get only 2 argument\n");
    } else {
        fdResults = open("results.csv", O_APPEND | O_CREAT | O_RDWR | O_TRUNC, S_IRWXU,S_IRWXO);
        if (fdResults < 0) {
            errorSystemCall();

        }
        fs = openFile(argv[1]);
        readConfigureFile(buffer, line, fs);
        if ((pDir = opendir(line[0])) == NULL) {
            errorSystemCall();
        }

        while ((pDirent = readdir(pDir)) != NULL) {
            buildPath(current,line[0],pDirent->d_name);

            if ((strcmp(pDirent->d_name, "..") == 0) ||
                (strcmp(pDirent->d_name, ".") == 0) ||
                !(isDirectory(line[0]))) {
                continue;
            }
            char *studentName = pDirent->d_name;
            StudentGrade studentGrade = searchCFile(
                    studentName,current,
                    line[1], line[2],&isFound);
            // write the result to result.csv
            int len = sprintf(s, "%s,%d,%s\n", studentGrade.name, studentGrade.grade, studentGrade.description);
            if (write(fdResults, s, (size_t) len) == -1 ){
                errorSystemCall();
            }
            isFound = 0;
        }
    }
    close(fdResults);
    close(fs);
    closedir(pDir);
}

void buildPath(char *current,const char* line,char* name){
    strcpy(current, line);
    strcat(current,"/");
    strcat(current,name);

}
int isExecutable(char *line) {
    const char *dot = strrchr(line, '.');
    if (dot != NULL) {
        if (!strcmp(dot, ".c")) {
            return 1;
        }
        return 0;
    }
}


int isDirectory(const char* path) {

    struct stat st;
    if (stat(path, &st) != 0)
        return 0;
    return S_ISDIR(st.st_mode);
}


void errorSystemCall() {
    char msg[] = "Error in system call\n";
    write(STD_ERR, msg, sizeof(msg));
    exit(EXIT_FAILURE);
}

int openFile(const char *filename) {
    int fileDescriptor = open(filename, O_RDONLY);
    if (fileDescriptor < 0) {
        errorSystemCall();
    }
    return fileDescriptor;
}

void readConfigureFile(char* configFile, char** line, int fs) {
    size_t position = 0;

    ssize_t numBytesOfRead = read(fs, configFile, SIZE_FILE);
    if (numBytesOfRead < 0) {
        errorSystemCall();
    }
    char *token = strtok(configFile, "\n");

    while (token != NULL) {
        line[position] = token;
        position++;
        token = strtok(NULL, "\n");
    }
}

StudentGrade runProgram(char* studentName, char* inputLocation, char* outputLocation, char* fileName){
    int fdOutputOfProgram,fdInput, dupStdout, dupStdin, status;
    int compareFlag, unlink1, unlink2;
    pid_t pid;
    pid_t state;
    StudentGrade studentGrade;
    studentGrade.grade = 0;
    studentGrade.name = NULL;
    char myOutputPath[SIZE_LINE];
    // create output file for the program that running in order to compare with it.
    strcpy(myOutputPath, studentName);
    strcat(myOutputPath, ".txt");
    fdOutputOfProgram = open(myOutputPath, O_APPEND | O_CREAT | O_RDWR | O_TRUNC, 0666);
    fdInput = open(inputLocation, O_RDONLY);
    if ( fdOutputOfProgram < 0 || fdInput < 0 ) {
        errorSystemCall();
    }



    pid = fork();
    if (pid < 0) {
        errorSystemCall();
    }
        // child .
    else if (pid == 0) {
        // running the program.
        // switch stdout, now ths standard output will go to 'fdOutputOfProgram'
        dupStdout = dup2(fdOutputOfProgram, STD_OUT);

        // switch stdin, we get the input from 'fdInput'
        dupStdin = dup2(fdInput, STD_IN);
        if (dupStdout < 0 || dupStdin < 0) {
            errorSystemCall();
        }
        // add "./" to begin.
        char execLine[SIZE_LINE];
        strcpy(execLine, "./");
        strcat(execLine, fileName);
        char *args[] = {fileName, NULL};
        execvp(execLine, args);
        exit(EXIT_FAILURE);
    } else {
            sleep(5);
        // check for TIMEOUT
            state = waitpid(pid, &status, WNOHANG);
        studentGrade.name = studentName;
        if(state == 0) {
            studentGrade.grade = 0;
            strcpy(studentGrade.description, TIMEOUT);
        } else {
            compareFlag = CompareFiles(outputLocation,myOutputPath);
            switch (compareFlag){
                case 1:
                    // different
                    studentGrade.grade = 60;
                    strcpy(studentGrade.description, BAD_OUTPUT);
                    break;
                case 2:
                    // similar
                    studentGrade.grade = 80;
                    strcpy(studentGrade.description, SIMILAR_OUTPUT);
                    break;
                case 3:
                    // identical.
                    studentGrade.grade = 100;
                    strcpy(studentGrade.description, GREAT_JOB);
                    break;
                default:
                    exit(EXIT_FAILURE);
            }
        }
        close(fdInput);
        close(fdOutputOfProgram);
        unlink1 = unlink(fileName);
        unlink2 = unlink(myOutputPath);
        if (unlink1 == -1 || unlink2 == -1) {
            errorSystemCall();
        }
    }
    return studentGrade;
}


int CompareFiles(char* file, char* output){
    pid_t pid;
    int status = 0;
    pid = fork();
    if (pid < 0) {
        errorSystemCall();

    } else if (pid == 0) {
        // child
        char* args[] = {"comp.out",file,output,NULL};
        execvp("./comp.out",args);
        exit(EXIT_FAILURE);
    } else {
        // main proccess.
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            // return the result.
            return WEXITSTATUS(status);
        }
        return -1;
    }
}


StudentGrade compileFile(char* studentName, char* path, char* inputLocation, char* outputLocation) {
    int status;
    char compiledFileName[SIZE_LINE];
    strcpy(compiledFileName, studentName);
    strcat(compiledFileName, ".out");
    char * args[] = {"gcc","-o", compiledFileName,path ,NULL};
    pid_t pid = fork();
    // child
    if(pid == 0) {
        execvp("gcc",args);
        exit(EXIT_FAILURE);
    }
        // fork error
    else if (pid < 0) {
        errorSystemCall();
        // wait.
    } else if (waitpid(pid, &status, 0) != pid) {
       errorSystemCall();
        // check if child process terminated normally.
    } else if (!WIFEXITED(status)) {
        errorSystemCall();
    } else if (WEXITSTATUS(status) == 1) {
            // there is compilation problem. if gcc failed it return 1.
            // therefore grade is 0.
            StudentGrade studentGrade = {studentName, COMPILATION_ERROR, 0};
            return studentGrade;
        }
        return runProgram(studentName, inputLocation, outputLocation, compiledFileName);
    }




StudentGrade searchCFile(char* studentName, char* dirLocation, char* inputLocation, char* outputLocation, int* isFound) {
    DIR *pDir;
    struct dirent *pDirent;
    char singleDirPath[MAX_INPUT];
    char temp[MAX_INPUT];
    // initilaize to "NO C FILES"
    StudentGrade studentGrade = {studentName, NO_C_FILE, 0};


    if ((pDir = opendir(dirLocation)) == NULL) {
        errorSystemCall();
    }
    // looping through the directory, printing the directory entry name
    while ((pDirent = readdir(pDir)) != NULL) {
        buildPath(temp,dirLocation, pDirent->d_name);
        if (isDirectory(temp) )
        {
            if (!strcmp(pDirent->d_name, ".") || !strcmp(pDirent->d_name, "..")) {
                continue;
            }

                strcpy(singleDirPath,temp);
                studentGrade = searchCFile(studentName, singleDirPath, inputLocation, outputLocation, isFound);
                if(*isFound) {
                    closedir(pDir);
                    return studentGrade;
                }

        } else if (isExecutable(pDirent->d_name)) {
            // change pointer to 1.
            *isFound =1;
            // compile file.
            studentGrade = compileFile(studentName, temp, inputLocation, outputLocation);
            closedir(pDir);
            return studentGrade;
        }
    }


    closedir(pDir);
    return studentGrade;
}















