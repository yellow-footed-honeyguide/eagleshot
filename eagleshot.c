#define _POSIX_C_SOURCE 200809L  // Set POSIX.1-2008 standard for advanced POSIX features

#include <errno.h>               // Include for error number definitions and handling
                                 // Used: errno, perror()

#include <fcntl.h>               // File control options for file descriptor operations
                                 // Used: O_RDONLY, O_WRONLY, O_CREAT (potentially)

#include <limits.h>              // For system-specific constants like PATH_MAX
                                 // Used: PATH_MAX

#include <signal.h>              // For signal handling functions and constants
                                 // Used: sigaction(), SIGINT, SIGTERM

#include <stdio.h>               // Standard I/O functions for file operations and printing
                                 // Used: printf(), fprintf(), snprintf(), perror()

#include <stdlib.h>              // For memory allocation, random numbers, and program control
                                 // Used: malloc(), free(), exit(), EXIT_FAILURE, EXIT_SUCCESS

#include <string.h>              // For string manipulation functions
                                 // Used: strcmp(), strftime(), memset()

#include <sys/stat.h>            // For file status and information functions
                                 // Used: mkdir(), stat(), S_ISDIR()

#include <sys/types.h>           // For system data types used in system calls
                                 // Used: pid_t

#include <sys/wait.h>            // For process control and waiting functions
                                 // Used: waitpid(), WIFEXITED(), WEXITSTATUS()

#include <time.h>                // For time-related functions and structures
                                 // Used: time(), localtime(), strftime()

#include <unistd.h>              // POSIX API for system calls and constants
                                 // Used: fork(), execvp(), pipe(), dup2(), close(), read()

#include "config.h"  // Include custom configuration file for project-specific settings

// Macro for error checking: executes 'call', prints error if it fails, and exits
#define CHECK(call)             \
    do {                        \
        if ((call) == -1) {     \
            perror(#call);      \
            exit(EXIT_FAILURE); \
        }                       \
    } while (0)

volatile sig_atomic_t keep_running = 1;  // Flag for main loop control, volatile for thread safety

// Signal handler function to gracefully stop the main loop
void signal_handler(int sig) {
    keep_running = 0;  // Set flag to stop main loop when signal is received
}

// Function to check if a directory exists at the given path
int dir_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0) &&
           S_ISDIR(st.st_mode);  // Check if path exists and is a directory
}

// Function to create a directory with read/write/execute permissions
void create_dir(const char *path) {
    CHECK(mkdir(path, 0755));  // Create directory, CHECK macro handles errors
}

// Function to get or create the Pictures directory in the user's home folder
char *get_pictures_dir() {
    const char *home = getenv("HOME");  // Get home directory path from environment
    if (!home) {
        fprintf(stderr, "Unable to get home directory\n");
        exit(EXIT_FAILURE);
    }
    char *pictures_dir = malloc(PATH_MAX);  // Allocate memory for pictures directory path
    if (!pictures_dir) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    snprintf(pictures_dir, PATH_MAX, "%s/Pictures", home);  // Create pictures directory path
    if (!dir_exists(pictures_dir)) {
        create_dir(pictures_dir);  // Create Pictures directory if it doesn't exist
    }
    return pictures_dir;
}

// Function to generate a unique filename based on current date and time
char *generate_filename() {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char *filename = malloc(64);  // Allocate memory for filename
    if (!filename) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strftime(filename, 64, "scr_%Y%m%d_%H%M%S.png", tm);  // Format filename with timestamp
    return filename;
}

// This function executes the 'slurp' command to allow the user to select a screen area.
// It returns the selected area's geometry as a string through the 'geometry' parameter.
int execute_slurp(char *geometry, size_t size) {
    // Create an array to hold two file descriptors for a pipe
    int pipefd[2];
    
    // Create a pipe for communication between parent and child processes
    if (pipe(pipefd) == -1) {
        perror("pipe");  // If pipe creation fails, print error message
        return -1;  // Return -1 to indicate failure
    }
    
    // Create a new process
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");  // If process creation fails, print error message
        return -1;  // Return -1 to indicate failure
    } else if (pid == 0) {  // This block runs in the child process
        // Close the read end of the pipe, we don't need it in the child
        close(pipefd[0]);
        
        // Redirect the standard output to the write end of the pipe
        // This means anything the child process prints will go into the pipe
        dup2(pipefd[1], STDOUT_FILENO);
        
        // Close the write end of the pipe, it's no longer needed after redirection
        close(pipefd[1]);
        
        // Replace the child process with the 'slurp' command
        // If this succeeds, the code below this line will never run
        execlp("slurp", "slurp", NULL);
        
        // If we get here, execlp failed
        perror("execlp");
        exit(EXIT_FAILURE);  // Exit the child process with a failure status
    } else {  // This block runs in the parent process
        // Close the write end of the pipe, we only need to read from it
        close(pipefd[1]);
        
        // Read the output of 'slurp' from the pipe
        // 'size - 1' ensures we leave space for a null terminator
        ssize_t bytes_read = read(pipefd[0], geometry, size - 1);
        
        // Close the read end of the pipe, we're done with it
        close(pipefd[0]);
        
        if (bytes_read <= 0) {
            return -1;  // If read failed or read nothing, return failure
        }
        
        // Null-terminate the string we read
        geometry[bytes_read] = '\0';
        
        // If the last character is a newline, remove it
        if (geometry[bytes_read - 1] == '\n') {
            geometry[bytes_read - 1] = '\0';
        }
        
        int status;
        // Wait for the child process to finish and get its exit status
        waitpid(pid, &status, 0);
        
        // Check if the child process exited normally and with a success status
        // Return 0 if successful, -1 if not
        return WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 0 : -1;
    }
}

// Function to execute the screenshot process using the selected area
void execute_screenshot(const char *pictures_dir, const char *filename) {
    char geometry[256];  // Array to store the selected area coordinates
    // Execute slurp to get the selected area, and check if it was successful
    if (execute_slurp(geometry, sizeof(geometry)) != 0) {
        fprintf(stderr,
                "Area selection cancelled or error occurred\n");  // Print error message to stderr
        return;  // Exit the function if area selection failed
    }
    pid_t pid = fork();           // Create a new process
    if (pid == -1) {              // Check if fork failed
        perror("fork");           // Print system error message
        exit(EXIT_FAILURE);       // Exit the program with failure status
    } else if (pid == 0) {        // This block runs in the child process
        char filepath[PATH_MAX];  // Array to store the full file path
        snprintf(filepath, PATH_MAX, "%s/%s", pictures_dir, filename);  // Create full filepath
        execlp("grim", "grim", "-g", geometry, filepath, NULL);         // Execute grim command
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {  // Parent process
        int status;
        waitpid(pid, &status, 0);  // Wait for child process to finish
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Error executing grim\n");
        } else {
            printf("Screenshot saved: %s/%s\n", pictures_dir, filename);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1 && (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)) {
        printf("eagleshot version %s\n", VERSION);  // Print version if requested
        return 0;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);   // Set up signal handler for SIGINT (Ctrl+C)
    sigaction(SIGTERM, &sa, NULL);  // Set up signal handler for SIGTERM

    char *pictures_dir = get_pictures_dir();  // Get or create Pictures directory
    char *filename = generate_filename();     // Generate unique filename for screenshot

    if (keep_running) {
        execute_screenshot(pictures_dir, filename);  // Take screenshot if not interrupted
    }

    free(pictures_dir);  // Free allocated memory for pictures directory path
    free(filename);      // Free allocated memory for filename

    return 0;
}
