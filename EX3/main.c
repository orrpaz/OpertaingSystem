#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

#define ERROR -1
#define BUFFER_SIZE 2
typedef enum difference {
     DIIFFERENT = 1, SIMILAR, IDENTICAL } Difference;

//int compare(int fileDescriptor1,int fileDescriptor2, size_t fileSize1,size_t fileSize2);
int compare(int fileDescriptor1,int fileDescriptor2);
int isBackSpaceOrNewLine(const char* buffer, int i);

/**
 * Get the size of a file.
 * @return The filesize, or 0 if the file does not exist.
 */
size_t getFilesize(const char* filename);



void errorSystemCall(){
    char msg[] = "Error in system call\n";
    write(2, msg, sizeof(msg));
    exit(EXIT_FAILURE);
}


int openFile(const char* filename){
    int fileDescriptor = open(filename, O_RDONLY);
    if (fileDescriptor < 0) {
        errorSystemCall();
        //return ERROR;
    }
    return fileDescriptor;
}

size_t getFilesize(const char* filename) {
    struct stat st;
    if(stat(filename, &st) < 0) {
        errorSystemCall();
    }
    return (size_t) st.st_size;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("The program get only 2 argument\n");
    } else if (!argv[1] || !argv[2]) { // Check if no argument is given.
            return ERROR;
    } else {
        int fileDescriptor1, fileDescriptor2;
//        size_t d= getFilesize(argv[1]);
//        printf("%zu",d);
        fileDescriptor1 = openFile(argv[1]);
        fileDescriptor2 = openFile(argv[2]);

//        size_t sizeFile1 = getFilesize(argv[1]);
//        size_t sizeFile2 = getFilesize(argv[2]);
//
//        int result_code = compare(fileDescriptor1,fileDescriptor2,sizeFile1,sizeFile2);
        int result_code = compare(fileDescriptor1,fileDescriptor2);
        if (result_code == IDENTICAL) {
            printf("The files are identical\n");
        } else if (result_code == SIMILAR)
        {
            printf("The files are similar\n");

        } else {
            printf("The files are different\n");

        }
        return result_code;

    }
}

//int compare(int fileDescriptor1,int fileDescriptor2, size_t fileSize1,size_t fileSize2){
//
//   int i=0,j=0;
//    ssize_t numBytesOfRead ,numBytesOfRead2;
//
//    char* tempBuffer1 = (char*)malloc((fileSize1) * sizeof(char));
//    if(!tempBuffer1) {
//        return ERROR;
//    }
//    char* tempBuffer2 =(char*)malloc((fileSize2) * sizeof(char));
//    if(!tempBuffer2) {
//        return ERROR;
//    }
//
//     numBytesOfRead = read(fileDescriptor1, tempBuffer1, fileSize1);
//     numBytesOfRead2 = read(fileDescriptor2, tempBuffer2, fileSize2);
//    if (numBytesOfRead < 0 || numBytesOfRead2 < 0) {
//        errorSystemCall();
//    }
//    // put '\0' in end of file.
////    tempBuffer1[fileSize1-1]='\0';
////    tempBuffer2[fileSize2-1]='\0';
//
//
//}

int compare(int fileDescriptor1,int fileDescriptor2){
    Difference result;
    char buffer1[BUFFER_SIZE + 1];
    char buffer2[BUFFER_SIZE + 1];
    int i = 0, j = 0;
    int flag = 1;
    memset(buffer1, 0, BUFFER_SIZE);
    memset(buffer2, 0, BUFFER_SIZE);


    ssize_t numBytesOfRead = 0, numBytesOfRead2 = 0;
    //numBytesOfRead = read(fileDescriptor1, buffer1, BUFFER_SIZE);
    //numBytesOfRead2 = read(fileDescriptor2, buffer2, BUFFER_SIZE);
   // if (numBytesOfRead < 0 || numBytesOfRead2 < 0) {
 //       errorSystemCall();
 //   }
    result = IDENTICAL;

        while(flag)
        {

            if(buffer1[i] == '\0')
            {
                i = 0;
                numBytesOfRead = read(fileDescriptor1, buffer1, BUFFER_SIZE);
                buffer1[numBytesOfRead] = '\0';
            }
            if(buffer2[j] == '\0')
            {
                j = 0;
                numBytesOfRead2 = read(fileDescriptor2, buffer2, BUFFER_SIZE);
                buffer2[numBytesOfRead2] = '\0';
            }

            if (numBytesOfRead < 0 || numBytesOfRead2 < 0)
            {
                   errorSystemCall();
            }

            else if (numBytesOfRead > 0 && numBytesOfRead2 > 0)
            {
                while (buffer1[i] != '\0' && buffer2[j] != '\0') {
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
                        // we want to skip the next letter that dosent ' ' or '\n'
                        // because we want to ignore it with them
                    } else {
                        result = SIMILAR;
                        if (isBackSpaceOrNewLine(buffer1, i)) {
                            ++i;
                        } if (isBackSpaceOrNewLine(buffer2, j)) {
                            ++j;
//                            result = SIMILAR;
                        } else {
                            result = DIIFFERENT;
                            flag = 0;
                            break;
                        }
                    }
                }
            }
            else
            {
                if(numBytesOfRead == 0 && numBytesOfRead2 == 0)
                {
                    flag = 0;
                    break;
                }
                else if(numBytesOfRead == 0)
                {
                   while(numBytesOfRead2 > 0)
                   {
                       if(buffer2[j] == '\0') {
                           j = 0;
                           if( 0 > (numBytesOfRead2 = read(fileDescriptor2, buffer2, BUFFER_SIZE)))
                           {
                               errorSystemCall();
                           }
                           buffer2[numBytesOfRead2] = '\0';
                           if (numBytesOfRead2==0){
                               flag=0;
                               break;
                           }

                       }
                       else if(buffer2[j] == ' ' || buffer2[j] =='\n')
                       {
                          j++;

                       }
                       else if(buffer2[j] == buffer1[i])
                       {
                           j++;
                           result=SIMILAR;
                       }
                       else
                       {
                           result=DIIFFERENT;
                           flag = 0;
                           break;
                       }
                   }
                }
                else if(numBytesOfRead2 == 0)
                {
                    while(numBytesOfRead > 0)
                    {
                        if(buffer1[i] == '\0')
                        {
                            i = 0;
                            if( 0 > (numBytesOfRead = read(fileDescriptor1, buffer1, BUFFER_SIZE)))
                            {
                                errorSystemCall();
                            }
                            buffer1[numBytesOfRead] = '\0';
                            if (numBytesOfRead==0){ flag=0;break; }

                        }
                        else if(buffer1[i] == ' ' || buffer1[i] =='\n')
                        {
                            i++;

                        }
                        else if(buffer2[j] == buffer1[i])
                        {
                            i++;
                            result=SIMILAR;
                        }
                        else
                        {
                            result=DIIFFERENT;
                            flag = 0;
                            break;
                        }
                    }
                }
            }

    }
    return result;
}

int isBackSpaceOrNewLine(const char* buffer, int i){
    return (buffer[i] == ' ' || buffer[i] == '\n');
}

//int getNext(int fs,char* buffer, int i) {
//    ssize_t bytes;
//    i++;
//    while(buffer[i] == ' ' || buffer[i] == '\n'){
//        if (buffer[i] != '\0') {
//            i++;
//        } else {
//            memset(buffer,0,BUFFER_SIZE);
//            bytes = read(fs, buffer, BUFFER_SIZE);
//            if (bytes > 0) {
//                i = -1;
//            }
//        }
//    }
//    return i;
//}