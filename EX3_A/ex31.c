// Or Paz
// 311250708

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#define ERROR (-1)
#define BUFFER_SIZE 2
typedef enum difference {
    DIFFERENT = 1, SIMILAR, IDENTICAL } Difference;
/**1 i
 * check if the next char is ' ' or new line
 * @param buffer the chars
 * @param i index
 * @return 1 if  the next char is ' ' or '\n'
 */
int isBackSpaceOrNewLine(const char* buffer, int i);
/**
 * this method compare between to files
 * @param fileDescriptor1 - file descriptor of file 1
 * @param fileDescriptor2 - file descriptor of file 2
 * @param fileSize1 - size of first file in byte
 * @param fileSize2 - size of second file in byte
 * @return return if the files is identical-3, similar-2 or different-1
 */
int compare(int fileDescriptor1,int fileDescriptor2, size_t fileSize1,size_t fileSize2);
//int compare(int fileDescriptor1,int fileDescriptor2);
/**
 * this method open file.
 * @param filename -the path
 * @return return file descriptor.
 */
int openFile(const char* filename);
/**
 * check if system call faild . if yes, we exit from program
 */
void errorSystemCall();
/**
 * this method return the size of file in bytes
 * @param filename -the path
 * @return the size of file in bytes
 */
size_t getFileSize(const char* filename);


void errorSystemCall(){
    char msg[] = "Error in system call\n";
    write(2, msg, sizeof(msg));
    exit(EXIT_FAILURE);
}

int isBackSpaceOrNewLine(const char* buffer, int i){
    return (buffer[i] == ' ' || buffer[i] == '\n');
}


int main(int argc, char *argv[]) {
    int result_code=0;
    int fileDescriptor1, fileDescriptor2;
    if (argc != 3) {
        printf("The program get only 2 argument\n");
    } else if (!argv[1] || !argv[2]) { // Check if no argument is given.
        return ERROR;
    } else {

//        size_t d= getFilesize(argv[1]);
//        printf("%zu",d);
        fileDescriptor1 = openFile(argv[1]);
        fileDescriptor2 = openFile(argv[2]);

        size_t sizeFile1 = getFileSize(argv[1]);
        size_t sizeFile2 = getFileSize(argv[2]);

        result_code = compare(fileDescriptor1, fileDescriptor2, sizeFile1, sizeFile2);
//        int result_code = compare(fileDescriptor1,fileDescriptor2);
        if (result_code == IDENTICAL) {
            printf("The files are identical\n");
        } else if (result_code == SIMILAR) {
            printf("The files are similar\n");

        } else {
            printf("The files are different\n");

        }


    }
    return result_code;
}

int openFile(const char* filename){
    int fileDescriptor = open(filename, O_RDONLY);
    if (fileDescriptor < 0) {
        errorSystemCall();
        //return ERROR;
    }
    return fileDescriptor;
}

size_t getFileSize(const char* filename) {
    struct stat st;
    if(stat(filename, &st) < 0) {
        errorSystemCall();
    }
    return (size_t) st.st_size;
}



int compare(int fileDescriptor1,int fileDescriptor2, size_t fileSize1,size_t fileSize2) {
    Difference result;

    int checkBuff1, checkBuff2;
    int i = 0, j = 0, flag = 1;
    ssize_t numBytesOfRead, numBytesOfRead2;

    char *buffer1 = (char *) malloc((fileSize1) * sizeof(char));
    if (!buffer1) {
        return ERROR;
    }
    char *buffer2 = (char *) malloc((fileSize2) * sizeof(char));
    if (!buffer2) {
        free(buffer1);
        return ERROR;
    }

    numBytesOfRead = read(fileDescriptor1, buffer1, fileSize1);
    numBytesOfRead2 = read(fileDescriptor2, buffer2, fileSize2);
    if (numBytesOfRead < 0 || numBytesOfRead2 < 0) {
        errorSystemCall();
    }
    // put '\0' in end of file.
//    buffer1[fileSize1] = '\0';
//    buffer2[fileSize2] = '\0';

    result = IDENTICAL;
    if (numBytesOfRead < 0 || numBytesOfRead2 < 0) {
        errorSystemCall();
    } else if (numBytesOfRead > 0 && numBytesOfRead2 > 0) {
        while (i < fileSize1 && j < fileSize2) {
            // continue till the end if its identical.
            if (buffer1[i] == buffer2[j]) {
                i++;
                j++;
                // the file doesnt identical - its different of similar
                // we arrive to ' ' or '\n' on one files.
            } else if (tolower(buffer1[i]) == tolower(buffer2[j])) {
                result = SIMILAR;
                i++;
                j++;
            } else {
                // we want to skip the next letter that dosent ' ' or '\n'
                // because we want to ignore it with them
                result = SIMILAR;
                checkBuff1 = isBackSpaceOrNewLine(buffer1, i);
                if (checkBuff1) {
                    ++i;
                }
                checkBuff2 = isBackSpaceOrNewLine(buffer2, j);
                if (checkBuff2) {
                    ++j;
                } else if (!checkBuff1) {
                    result = DIFFERENT;
                    break;
                }

            }


        }

    }
    if(result != DIFFERENT){
    // file 2 end, run file 1
        if (j == fileSize2) {

            while (i < fileSize1) {
                //read backspace and /n

                if (isBackSpaceOrNewLine(buffer1, i)) {
                    result = SIMILAR;
                    i++;
                    continue;
                }
                if (buffer2[j] == buffer1[i]) {
                    i++;
                    result = SIMILAR;
                } else {
                    result = DIFFERENT;
                    break;
                }
            }

            // file 1 end, run file 2
        } else if (i == fileSize1) {

            while (j < fileSize2) {
                //read backspace and /n
                if (isBackSpaceOrNewLine(buffer2, j)) {
                    result = SIMILAR;
                    j++;
                    continue;
                }
                if (buffer2[j] == buffer1[i]) {
                    j++;
                    result = SIMILAR;
                } else {
                    result = DIFFERENT;
                    break;
                }
            }
        }
    }
    if (close(fileDescriptor1) == -1) {
        errorSystemCall();
    }
    if (close(fileDescriptor2) == -1) {
        errorSystemCall();
    }
    free(buffer1);
    free(buffer2);
    return result;
}





