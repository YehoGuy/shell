#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>
#include <errno.h>
#include "LineParser.h" // Assuming LineParser.c and LineParser.h are in the same directory

#define MAX_INPUT_SIZE 2048

void execute(cmdLine *pCmdLine) {
    if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
        perror("Execution failed");
        _exit(1); // Exit abnormally in case of execvp failure
    }
}

int main() {
    char cwd[PATH_MAX]; // Buffer to store the current working directory
    char input[MAX_INPUT_SIZE]; // Buffer to store user input
    cmdLine *parsedLine;

    while (1) {
        // Display the prompt (current working directory)
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("getcwd failed");
            continue;
        }
        printf("%s> ", cwd);

        // Read input from the user
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            perror("fgets failed");
            continue;
        }

        // Remove the trailing newline character,
        // because execvp expects the command string and arguments to be clean, 
        // without newline characters.
        input[strcspn(input, "\n")] = '\0';

        // Check if the input is "quit"
        if (strcmp(input, "quit") == 0) {
            break; // Exit the shell loop
        }

        // Parse the input
        parsedLine = parseCmdLines(input);
        if (parsedLine == NULL) {
            fprintf(stderr, "Failed to parse command\n");
            continue;
        }

        // Fork a new process to execute the command
        int pid = fork();
        if (pid == -1) {
            perror("Fork failed");
            freeCmdLines(parsedLine);
            continue;
        }

        if (pid == 0) {
            // Child process: execute the command
            execute(parsedLine);
        } else {
            // Parent process: wait for the child process to complete
            waitpid(pid, NULL, 0);
        }

        // Release the parsed command resources
        freeCmdLines(parsedLine);
    }

    return 0; // Exit the shell "normally"
}
