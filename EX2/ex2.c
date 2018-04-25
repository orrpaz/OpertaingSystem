// Or Paz
// 311250708


#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define PROMPT "prompt> "
#define MAX_COMMAND_LEN 512
#define MAX_ARGS_COMMAND_LEN 512
#define PATH_SIZE 1024
#define MAX_JOBS 100



typedef enum State {
    WAIT, DONTWAIT, NONE
} State;

typedef enum cmd {
     CD, JOBS, EXIT, ECHO, OTHER
} cmd;

typedef struct job {
    pid_t pid;      /* PID of the job */
    State state;    /* state of the job */
    char command[MAX_COMMAND_LEN];      /* command to print */
} job;

typedef struct info {
    job* jobs;
    size_t maxJobs;
}info;


/**
 * read a line from prompt
 * @param str - pointer to line
 * @param n -size
 * @param stream -stdin
 */
static void readline(char *str, int n, FILE *stream);
/**
 * parse the commend with "" and " ".
 * @param words the line
 * @param line
 * @return number of args
 */
size_t parseline(char **words,char *line);
/**
 * check if thre is & in the end of commend
 * @param command -command
 * @param n -size
 * @return return state - WAIT or DONTWAIT (for &)
 */
State isBackground(char **command, size_t n);
/**
 * check if the caommand is cd,exit,jobs or other
 * return enum that symbolize thr command.
 */
cmd specialCommand(char *command);
/**
 * this function handle cd command
 * @param args - all command.
 * @param numOfelement - number of element in command
 * @return return always 1.
 */
int changeDirectory(char **args,size_t numOfElement);
/**
 * display jobs in screen
 * @param args - all command
 * @param information struct that contain jobs and size.
 * @return always 1.
 */
int listOfJobs(char **args, info *information);
/**
 * kill all process and exit
 * @param information struct that contain jobs and size.
 * @return 0.
 */
int exitFromShell(info *information);
/**
 * execute the OTHER command.
 * @param input - all line(command)
 * @param command -just the first args.
 * @param  information struct that contain jobs and size.
 * @param state - WAIT or DONTWAIT.
 * @param numberOfElement - number of element in command
 * @return always 1.
 */
int run(char **input,char* command, info *information,State state, size_t numberOfElement);
/**
 *
 * @param c -  enum that symbolize the special command.
 * @param input all command (line)
 * @param command -just the first args.
 * @param information struct that contain jobs and size.
 * @param numberOfElement - number of element in command
 * @param state - WAIT or DONTWAIT.
 * @return 0 in exit and 1 otherwise
 */
int executeCommand(cmd c, char **input,char* command, info *information,size_t numOfElement,State state);
/**
 *
 * @param information struct that contain jobs and size.
 * @param pid pid
 * @param state - WAIT or DONTWAIT.
 * @param command -just the first args.
 * @return always 1
 */
int addJob(info *information, pid_t pid,State state, char* command);
/**
 * initilazie the jobs array
 * @param information struct that contain jobs and size.
 * @param initialIndex - index that symbolize from where to start the init.
 */
void initalizeJobs(info *information, int initialIndex);

static char prev[PATH_SIZE]="";


static void readline(char *str, int n, FILE *stream)
{
    fgets(str, n, stream);
    str[strlen(str) - 1] = '\0';
}




size_t parseline(char **words,char *line) {
    size_t position = 0;
    //char *ptr;
    size_t i=0;
    char *token = strtok(line, " ");
    if (token) {
        words[position] = token;
        line += (strlen(token) + 1);

            while (token != NULL) {
                if (line) {
                    if (*line == '"') {
                        ++position;
                        token = strtok(NULL, "\"");
                        if (token)
                            words[position] = token;
                    } else {
                        ++position;
                        token = strtok(NULL, " ");
                        if (token) {
                            words[position] = token;

                            line += (strlen(token) + 1);
                        }
                    }
                }

            }
    }
    words[position]=NULL;
    return position;
}




    State isBackground(char **command, size_t n) {
        int i = 0;
        State retState = WAIT;
        for (; i < n; ++i) {
            if (!strcmp(command[i], "&")) {
                retState = DONTWAIT;
                break;
            }
        }
        return retState;
    }

    cmd specialCommand(char *command) {

        int i = 4;
        static char *commands[] = {"cd", "jobs", "exit", "echo"};
        if (command) {
            for (i = 0; i < sizeof(commands) / sizeof(char *); ++i) {
                if (!strcmp(command, commands[i])) {
                    break;
                }
            }
        }
        return (cmd) i;

    }

    int changeDirectory(char **args, size_t numOfElement) {
        printf("%d\n", getpid());
        // Home directory
        if((numOfElement==1) || !strcmp(args[1], "~")) {
            if (getcwd(prev, PATH_SIZE) == NULL) {
                fprintf(stderr, "Error in system call\n");
            }
            if (chdir(getenv("HOME")) != 0) {
                fprintf(stderr, "Error in system call\n");

            }
             // '-' argument
        }else if(!strcmp(args[1], "-")){
            char cwd[PATH_SIZE];
            getcwd(cwd, PATH_SIZE);
                if(!strcmp(prev,"")) {
                    fprintf(stderr, "bash: cd: OLDPWD not set\n");
                }else if (chdir(prev) != 0) {
                    fprintf(stderr, "Error in system call\n");
                } else {
                    printf("%s\n",prev);
                }
            strcpy(prev, cwd);
            } else {
                    if(getcwd(prev,PATH_SIZE) == NULL){
                        fprintf(stderr, "Error in system call\n");
                    }
            // otherwise.
                     if(chdir(args[1])!= 0) {
                        fprintf(stderr, "Error in system call\n");
                }
            }
        return 1;
        }


    int listOfJobs(char **args, info *information) {
        int i, status;
        for (i = 0; i < information->maxJobs; i++) {
            // introduce only active process.
            if (information->jobs[i].pid != -1)
                if (waitpid(information->jobs[i].pid, &status, WNOHANG) == 0) {
                    printf("%d ", information->jobs[i].pid);
                    printf("%s  \n", information->jobs[i].command);
                } else {
                    information->jobs[i].pid = -1;
                }
        }
        return 1;
    }

    int exitFromShell(info *information) {
        int i;
        for (i = 0; i < information->maxJobs; ++i) {
            // kill process before exit.
            if(kill(information->jobs[i].pid, 0) == -1)
            {
                fprintf(stderr, "Error in system call\n");
                exit(1);
            }
        }
        return 0;
    }

    int run(char **input, char *command, info *information, State state, size_t numberOfElement) {
        int status = 0;
        pid_t pid, wpid;


        // replace & with \0
        if (state) {
            input[numberOfElement - 1] = '\0';
        }
        pid = fork();
        if (pid == 0) {
            // Child process
            if (execvp(input[0], input) == -1) {
                fprintf(stderr, "Error in system call\n");
                exit(1);
            }
            return 0;
        } else if (pid < 0) {
            // Error forking
            fprintf(stderr, "Fork error\n");
            return 1;
        } else {
            // Parent process
            do {
                printf("%d\n", pid);
                if (state == WAIT) {
                    wpid = waitpid(pid, &status, WUNTRACED);
                } else {
                    command[strlen(command) - 1] = '\0';
                    addJob(information, pid, state, command);
                }
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));

        }
        return 1;
    }



    int addJob(info *information, pid_t pid, State state, char *command) {
        int i = 0;
        int status;
        while (i < information->maxJobs) {
            if (information->jobs[i].pid == -1 || waitpid(information->jobs[i].pid, &status, WNOHANG) != 0) {
                information->jobs[i].pid = pid;
                information->jobs[i].state = state;
                strcpy(information->jobs[i].command, command);
                break;
            }
            i++;
            // resize of arrays jobs
            if (i == information->maxJobs) {
                size_t n = information->maxJobs * 2;
                job *ptr;
                ptr = realloc(information->jobs, n * sizeof(job));
                if (ptr) {
                    size_t temp = information->maxJobs;
                    information->maxJobs = n;
                    information->jobs = ptr;
                    initalizeJobs(information, (int) temp);
                    information->maxJobs = n;
                }
            }
        }
    }


    int executeCommand(cmd c, char **input, char *command, info *information, size_t numOfElement, State state) {
        int i;
        switch (c) {
            case CD:
                i = changeDirectory(input, numOfElement);
                break;
            case JOBS:
                i = listOfJobs(input, information);
                break;
            case EXIT:
                i = exitFromShell(information);
                break;
            case ECHO:
            case OTHER:
                i = run(input, command, information, state, numOfElement);
                break;
            default:
                i = 0;
                break;
        }
        return i;
    }

    void initalizeJobs(info *information, int initialIndex) {
        int i;
        for (i = initialIndex; i < information->maxJobs; i++) {
            strcpy(information->jobs[i].command, "");
            information->jobs[i].pid = -1;
            information->jobs[i].state = NONE;
        }
    }


    int main() {
        char command[MAX_COMMAND_LEN];
        int flag = 1;
        int initialIndex = 0;
        char *argCommand[MAX_ARGS_COMMAND_LEN];
        size_t numOfWordPerCommand = 0;
        cmd c;
        State state = NONE;
        info *information = malloc(sizeof(info));
        if (information == NULL) {
            exit(1);
        }
        information->maxJobs = MAX_JOBS;
        information->jobs = (void *) calloc(MAX_JOBS, sizeof(job));
//            job *jobs = (void *) calloc(MAX_JOBS, sizeof(job));
        if (information->jobs == NULL) {
            fprintf(stderr, "allocation error\n");
            exit(EXIT_FAILURE);
        }
        initalizeJobs(information, initialIndex);
        while (flag) {
            printf(PROMPT);
            readline(command, MAX_COMMAND_LEN, stdin);
            char commandCpy[MAX_COMMAND_LEN];
            strcpy(commandCpy, command);
            numOfWordPerCommand = parseline(argCommand, command);
            state = isBackground(argCommand, numOfWordPerCommand);
            c = specialCommand(argCommand[0]);
            flag = executeCommand(c, argCommand, commandCpy, information, numOfWordPerCommand, state);

        }
        free(information->jobs);
        free(information);


    }




