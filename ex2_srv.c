#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#define MAX_FILENAME_LENGTH 30

pid_t pid_client;
extern int errno;
void handler(int sig) {

    signal(SIGUSR1, handler);
    sleep(1);
    FILE* fd = fopen("to_srv.txt", "r");
    if (fd == NULL) {
        perror("Failing to open to_srv.txt");
        printf("%d", errno);
        exit(-1);
    }

    char pid_str[10];
    char operation;
    char a_str[10], b_str[10];
    int a, b, pid_client;

    if ((fscanf(fd, "%s %s %c %s", pid_str, a_str, &operation, b_str) != 4)) {
        perror("Failed to read data from to_srv.txt");
        exit(-1);
    }

    fclose(fd);

    pid_client = atoi(pid_str);
    a = atoi(a_str);
    b = atoi(b_str);
    printf("server received data from process %d\n", pid_client);
    printf("Client PID: %d\n", pid_client);
    printf("Operation: %c\n", operation);
    printf("a : %d, b:%d\n", a, b); /

        pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
    }
    else if (pid == 0) {
        printf("Chid process PID: %d\n", getpid());

        printf("remove to_srv.txt\n");
        if (remove("to_srv.txt") != 0) {
            perror("Failed to remove to_srv.txt");
            exit(-1);
        }
        printf("to_srv removed\n");

        printf("open to_client file\n");
        char filename[MAX_FILENAME_LENGTH];
        sprintf(filename, "to_client_%d.txt", pid_client);
        int fd_client = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (fd_client == -1) {
            perror("Failed to create to_client file");
            exit(-1);
        }
        int result;

        switch (operation) {
        case '1':
            result = a + b;
            break;
        case '2':
            result = a - b;
            break;
        case '3':
            result = a * b;
            break;
        case '4':
            if (b == 0) {
                sprintf(filename, "to_client_%d.txt", pid_client);
                fd_client = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                if (fd_client == -1) {
                    perror("Failed to open to_client file");
                    exit(-1);
                }
                write(fd_client, "Cannot divide by zero\n", strlen("Cannot divide by zero\n"));
                printf("Cannot divide by zero");
                close(fd_client);
                exit(-1);
            }
            else {
                result = a / b;
            }
            break;
        default:
            printf("You did not enter a valid operation");
            exit(-1);
        }

        printf("Write result to_client_%d.txt: %d\n", pid_client, result);

        dprintf(fd_client, "%d\n", result);

        if (fsync(fd_client) == -1) {
            perror("fsync failed");
        }

        close(fd_client);
        printf("sending signal - the child finish\n");
        kill(pid_client, SIGUSR1);
        exit(0);


    }
    else {
        printf("parent wait for son\n");
        wait(NULL);
    }

}
int main() {

    printf("PID SRV - %d\n", getpid());

    signal(SIGUSR1, handler);

    while (1) {
        pause();

        //clear the zumbies
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
    return 0;
}