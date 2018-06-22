// Or Paz
// 311250708

#include <termio.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define STD_ERR 2
/**
 * error sys call.
 */
void errorSystemCall() {
    char msg[] = "Error in system call\n";
    write(STD_ERR, msg, sizeof(msg));
    exit(EXIT_FAILURE);
}

/**
 * get char.
 */
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}
/**
 * this method return 1 if the key was insertes is one of: a,d,c,w,q
 */
int isLegalKey(char ch) {
    return (ch == 'a' || ch == 'd' || ch == 's' || ch == 'w' || ch == 'q');
}
/**
 * do fork and all draw.out
 */
int main() {
    int fd[2];
    char ch;
    pipe(fd);
    int pid;
    pid = fork();
    if (pid < 0) {
        errorSystemCall();
    } else {
        // child proccess.
        if (pid == 0) {
            if (close(fd[1]) == -1) {
                errorSystemCall();
            }
            // change stdtin to fd[0].
            dup2(fd[0], 0);
            char *args[] = {"./draw.out", NULL};
            execvp(args[0], args);
            exit(EXIT_FAILURE);
        } else {
            // father proccess.
            if (close(fd[0]) == -1) {
                errorSystemCall();
            }
            while (1) {
                ch = getch();
                if (!isLegalKey(ch)) {
                    continue;
                }
                // write via pipe
                if (write(fd[1], &ch, 1) < 0) {
                    errorSystemCall();
                }
                // send SIGUSR2
                if (kill(pid, SIGUSR2) == -1) {
                    errorSystemCall();
                }

                if (ch == 'q') {
                    break;
                }
            }
            if (close(fd[1]) == -1) {
                errorSystemCall();
            }
            return 0;
        }
    }
}
