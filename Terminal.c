#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>

char **getLine() {
    char **strings;
    while (true) {
        int currentChar = getchar();
        if ((currentChar == "\n") || (currentChar == EOF)) {
            break;
        }
//        else if()

    }
    return strings;
}

int main(int argc, char *argv[]) {
    pid_t pid = fork();
    int status;
    if (pid == 0) {
        while (true) {
            char **strings = getLine();
            if (execve(argv[1], argv + 1, NULL) < 0) {
                perror("execve failed");
            }
        }
    } else if (pid > 0) {
        waitpid(pid, &status, 0);
    } else {
        perror("fork failed");
    }
    return 0;
}