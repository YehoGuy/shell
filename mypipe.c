#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 256

int main() {
    int pipe_fd[2]; // Array to hold the two ends of the pipe: [0] for reading, [1] for writing
    int pid;

    // Create the pipe
    if (pipe(pipe_fd) == -1) {
        perror("Pipe creation failed");
        return 1;
    }

    // Fork a child process
    pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        return 1;
    }

    if (pid == 0) {
        // Child process
        close(pipe_fd[0]); // Close the read end of the pipe

        const char *message = "hello";
        if (write(pipe_fd[1], message, strlen(message)) == -1) {
            perror("Write to pipe failed");
            _exit(1);
        }

        close(pipe_fd[1]); // Close the write end after sending the message
        _exit(0);          // Terminate child process
    } else {
        // Parent process
        close(pipe_fd[1]); // Close the write end of the pipe

        char buffer[BUFFER_SIZE] = {0};
        ssize_t bytes_read = read(pipe_fd[0], buffer, sizeof(buffer) - 1);
        if (bytes_read == -1) {
            perror("Read from pipe failed");
            return 1;
        }

        // Null-terminate the message and print it
        buffer[bytes_read] = '\0';
        printf("Received message: %s\n", buffer);

        close(pipe_fd[0]); // Close the read end of the pipe
    }

    return 0;
}
