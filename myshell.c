#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>
#include <errno.h>
#include "LineParser.h" 
#include <signal.h> // Added for signal handling

#define MAX_INPUT_SIZE 2048

int debug_mode = 0;

void execute(cmdLine *pCmdLine) {
    if (execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1) {
        perror("Execution failed");
        _exit(1); // Exit abnormally in case of execvp failure
    }
}

void handle_signal_command(cmdLine *parsedLine) {
    if (parsedLine->argCount < 2) {
        fprintf(stderr, "Error: Missing PID argument\n");
        return;
    }

    int pid = atoi(parsedLine->arguments[1]); // Convert PID argument to an integer
    if (pid <= 0) {
        fprintf(stderr, "Error: Invalid PID\n");
        return;
    }

    if (strcmp(parsedLine->arguments[0], "stop") == 0) {
        // Send SIGSTOP signal
        if (kill(pid, SIGSTOP) == -1) {
            perror("Failed to send SIGSTOP");
        } else if (debug_mode == 1) {
            fprintf(stderr, "Sent SIGSTOP to process %d\n", pid);
        }
    } else if (strcmp(parsedLine->arguments[0], "wake") == 0) {
        // Send SIGCONT signal
        if (kill(pid, SIGCONT) == -1) {
            perror("Failed to send SIGCONT");
        } else if (debug_mode == 1) {
            fprintf(stderr, "Sent SIGCONT to process %d\n", pid);
        }
    } else if (strcmp(parsedLine->arguments[0], "term") == 0) {
        // Send SIGINT signal
        if (kill(pid, SIGINT) == -1) {
            perror("Failed to send SIGINT");
        } else if (debug_mode == 1) {
            fprintf(stderr, "Sent SIGINT to process %d\n", pid);
        }
    } else {
        fprintf(stderr, "Error: Unknown signal command\n");
    }
}




int main(int argc, char **argv) {
    char cwd[PATH_MAX]; // Buffer to store the current working directory
    char input[MAX_INPUT_SIZE]; // Buffer to store user input
    cmdLine *parsedLine;

     // Check for debug mode (-d flag)
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        debug_mode = 1;
        fprintf(stderr, "Debug mode activated\n");
    }

   

    while (1) {
        // Display the prompt (current working directory)
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            if(debug_mode==1)
                perror("getcwd failed");
            continue;
        }
        printf("%s> ", cwd);

        // Read input from the user
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            if(debug_mode==1)
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
            if(debug_mode==1)
                fprintf(stderr, "Failed to parse command\n");
            continue;
        }

        // handle cd in the parent process
        if (strcmp(parsedLine->arguments[0], "cd") == 0) {
            if (parsedLine->argCount < 2) {
                fprintf(stderr, "cd: Missing argument\n");
            } else if (chdir(parsedLine->arguments[1]) == -1) {
                if(debug_mode == 1)
                    perror("cd failed");
            } else if (debug_mode == 1) {
                fprintf(stderr, "Directory changed to: %s\n", parsedLine->arguments[1]);
            }
            continue;
        }

        // Handle signal commands: stop, wake, term
        if (strcmp(parsedLine->arguments[0], "stop") == 0 ||
            strcmp(parsedLine->arguments[0], "wake") == 0 ||
            strcmp(parsedLine->arguments[0], "term") == 0) {
            handle_signal_command(parsedLine); // Handle the signal command
            freeCmdLines(parsedLine);
            continue;
        }

        // Fork a new process to execute the command
        int pid = fork();
        if (pid == -1) {
            if(debug_mode==1)
                perror("Fork failed");
            freeCmdLines(parsedLine);
            continue;
        }

        if (pid == 0) {
            // Child process: execute the command
            execute(parsedLine);
        } else {
            //1a?
            if (debug_mode==1) {
                fprintf(stderr, "PID: %d\n", pid);
                fprintf(stderr, "Executing command: %s\n", parsedLine->arguments[0]);
            }
            // Parent process: wait for the child process to complete
            if(parsedLine->blocking){
                waitpid(pid, NULL, 0);
            }
        }

        // Release the parsed command resources
        freeCmdLines(parsedLine);
    }

    return 0; // Exit the shell "normally"
}
