#include <stdlib.h>
#include <stdio.h>
#include <sys/signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define MAX_ATTEMPTS 10
extern int errno;

void handler(int sig) {


    pid_t client_pid = getpid();
    char filename[30];

    signal(SIGUSR1, handler);
    sleep(1);
    sprintf(filename, "to_client_%d.txt", client_pid);
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open to_client file for reading");
        printf("%d", errno);
        exit(-1);
    }
    char result_str[30] = { 0 };
    ssize_t bytes_read = read(fd, result_str, sizeof(result_str) - 1); if (bytes_read == -1) {
        perror("Failed to read result from file\n");
        exit(-1);
    }
    result_str[bytes_read] = '\0';

    printf("data: %s\n", result_str);
    int result = atoi(result_str);
    if (result != 0 || strcmp(result_str, "0") == 0) {
        printf("Result: %d\n", result);
    }
    else {
        printf("Error: %s\n", result_str);
    }
    // printf("Result: %d\n", result); // Print the result to stdout
    close(fd);

}
int main(int argc, char* argv[]) {
    if (argc != 5) {
        perror("You didn't enter 4 numbers\n");
        exit(-1);
    }
    int server_pid = atoi(argv[1]);
    int a = atoi(argv[2]);
    int operation = atoi(argv[3]);
    int b = atoi(argv[4]);

    // Set signal handler
    signal(SIGUSR1, handler);

    pid_t client_pid = getpid();char filename[30];

    signal(SIGUSR1, handler);
    sleep(1);
    sprintf(filename, "to_client_%d.txt", client_pid);
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open to_client file for reading");
        printf("%d", errno);
        exit(-1);
    }
    char result_str[30] = { 0 };
    ssize_t bytes_read = read(fd, result_str, sizeof(result_str) - 1);
    if (bytes_read == -1) {
        perror("Failed to read result from file\n");
        exit(-1);
    }
    result_str[bytes_read] = '\0';

    printf("data: %s\n", result_str);
    int result = atoi(result_str);
    if (result != 0 || strcmp(result_str, "0") == 0) {
        printf("Result: %d\n", result);
    }
    else {
        printf("Error: %s\n", result_str);
    }
    // printf("Result: %d\n", result); // Print the result to stdout
    close(fd);

}
int main(int argc, char* argv[]) {
    if (argc != 5) {
        perror("You didn't enter 4 numbers\n");
        exit(-1);
    }
    int server_pid = atoi(argv[1]);
    int a = atoi(argv[2]);
    int operation = atoi(argv[3]);
    int b = atoi(argv[4]);

    // Set signal handler
    signal(SIGUSR1, handler);

    pid_t client_pid = getpid();
    printf("Client pid %d\n", client_pid);

    // Attempt to create "to_srv" file
    int attempt = 1;
    int fdout;
    while (attempt <= MAX_ATTEMPTS) {
        fdout = open("to_srv.txt", O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        if (fdout != -1) {
            // File created successfully
            break;
        }
        else {
            if (attempt == MAX_ATTEMPTS) {
                perror("Failed to create to_srv file after 10 attempts");
                exit(-1);
            }
            int rand_time = rand() % 5 + 1; // Generate a random time between 1 and 5 seconds
            printf("Attempt %d failed. Retrying after %d seconds.\n", attempt, rand_time); sleep(rand_time);
            attempt++;
        }
    }

    if (chmod("to_srv.txt", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) == -1) {
        perror("chmod error");
        exit(-1);
    }
    // Write data to the file
    dprintf(fdout, "%d\n%d\n%d\n%d\n", client_pid, a, operation, b);

    // Close the file
    close(fdout);
    sleep(1);

    // Send signal to the server
    kill(server_pid, SIGUSR1);

    // Pause execution until the signal is received
    pause();

    return 0;
}