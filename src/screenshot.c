#include <errno.h>     // errno (variable), perror (function)
#include <fcntl.h>     // O_RDONLY, O_WRONLY, O_CREAT (macros for file operations)
#include <limits.h>    // PATH_MAX (macro)
#include <stdio.h>     // fprintf, printf, snprintf, perror (functions)
#include <stdlib.h>    // exit, EXIT_FAILURE (macro)
#include <string.h>    // (potentially used string manipulation functions)
#include <sys/wait.h>  // waitpid, WIFEXITED, WEXITSTATUS (macros)
#include <unistd.h>    // fork, pipe, close, dup2, read, execlp (functions)

#include "screenshot.h"  // execute_slurp, execute_screenshot (functions)
#include "utils.h"       // (potentially used utility functions)

// Function to execute slurp command and capture its output
int execute_slurp(char *geometry, size_t size) {
    int pipefd[2];  // Array to hold file descriptors for pipe

    // Create a pipe for inter-process communication
    if (pipe(pipefd) == -1) {
        perror("pipe");  // Print error message if pipe creation fails
        return -1;       // Return error code
    }

    pid_t pid = fork();  // Create a child process

    if (pid == -1) {                     // Check if fork failed
        perror("fork");                  // Print error message
        return -1;                       // Return error code
    } else if (pid == 0) {               // Child process
        close(pipefd[0]);                // Close read end of pipe
        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to pipe
        close(pipefd[1]);                // Close write end of pipe
        execlp("slurp", "slurp", NULL);  // Execute slurp command
        perror("execlp");                // Print error if execlp fails
        exit(EXIT_FAILURE);              // Exit child process with failure status
    } else {                             // Parent process
        close(pipefd[1]);                // Close write end of pipe
        // Read slurp output from pipe into geometry buffer
        ssize_t bytes_read = read(pipefd[0], geometry, size - 1);
        close(pipefd[0]);  // Close read end of pipe

        if (bytes_read <= 0) {  // Check if read failed or no data
            return -1;          // Return error code
        }

        geometry[bytes_read] = '\0';             // Null-terminate the string
        if (geometry[bytes_read - 1] == '\n') {  // Remove newline if present
            geometry[bytes_read - 1] = '\0';
        }

        int status;
        waitpid(pid, &status, 0);  // Wait for child process to finish
        // Return 0 if child exited normally with status 0, else -1
        return WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 0 : -1;
    }
}

// Function to execute the screenshot process
void execute_screenshot(const char *pictures_dir, const char *filename) {
    char geometry[256];  // Buffer to store selected area geometry
    // Execute slurp to get area selection
    if (execute_slurp(geometry, sizeof(geometry)) != 0) {
        fprintf(stderr, "Area selection cancelled or error occurred\n");
        return;  // Exit function if slurp failed
    }
    pid_t pid = fork();           // Create a child process
    if (pid == -1) {              // Check if fork failed
        perror("fork");           // Print error message
        exit(EXIT_FAILURE);       // Exit program with failure status
    } else if (pid == 0) {        // Child process
        char filepath[PATH_MAX];  // Buffer for full file path
        // Construct full filepath
        snprintf(filepath, PATH_MAX, "%s/%s", pictures_dir, filename);
        // Execute grim command with selected geometry and filepath
        execlp("grim", "grim", "-g", geometry, filepath, NULL);
        perror("execlp");    // Print error if execlp fails
        exit(EXIT_FAILURE);  // Exit child process with failure status
    } else {                 // Parent process
        int status;
        waitpid(pid, &status, 0);  // Wait for child process to finish
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Error executing grim\n");  // Print error message
        } else {
            // Print success message with saved screenshot path
            printf("Screenshot saved: %s/%s\n", pictures_dir, filename);
        }
    }
}
