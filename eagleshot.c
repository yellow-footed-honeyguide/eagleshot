#define _POSIX_C_SOURCE 200809L  // Set POSIX.1-2008 standard
#include <errno.h>               // Error number definitions
#include <fcntl.h>               // File control options
#include <limits.h>              // Implementation-defined constants
#include <signal.h>              // Signal handling
#include <stdio.h>               // Standard I/O functions
#include <stdlib.h>              // Standard library functions
#include <string.h>              // String handling functions
#include <sys/stat.h>            // File status and information
#include <sys/types.h>           // Data types used in system calls
#include <sys/wait.h>            // Process control
#include <time.h>                // Time and date functions
#include <unistd.h>              // POSIX operating system API

#include "config.h"  // Custom configuration file

// Macro for error checking
#define CHECK(call)             \
    do {                        \
        if ((call) == -1) {     \
            perror(#call);      \
            exit(EXIT_FAILURE); \
        }                       \
    } while (0)

volatile sig_atomic_t keep_running = 1;  // Flag for main loop control

// Signal handler function
void signal_handler(int sig) {
    keep_running = 0;  // Set flag to stop main loop
}

// Check if directory exists
int dir_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0) && S_ISDIR(st.st_mode);
}

// Create directory
void create_dir(const char *path) {
    CHECK(mkdir(path, 0755));  // Create directory with read/write/execute permissions
}

// Get pictures directory path
char *get_pictures_dir() {
    const char *home = getenv("HOME");  // Get home directory
    if (!home) {
        fprintf(stderr, "Unable to get home directory\n");
        exit(EXIT_FAILURE);
    }
    char *pictures_dir = malloc(PATH_MAX);  // Allocate memory for path
    if (!pictures_dir) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    snprintf(pictures_dir, PATH_MAX, "%s/Pictures", home);  // Create pictures directory path
    if (!dir_exists(pictures_dir)) {
        create_dir(pictures_dir);  // Create directory if it doesn't exist
    }
    return pictures_dir;
}

// Generate unique filename based on current time
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

// Execute slurp command to select screen area
int execute_slurp(char *geometry, size_t size) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return -1;
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    } else if (pid == 0) {  // Child process
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to pipe
        close(pipefd[1]);
        execlp("slurp", "slurp", NULL);  // Execute slurp command
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {  // Parent process
        close(pipefd[1]);
        ssize_t bytes_read = read(pipefd[0], geometry, size - 1);  // Read slurp output
        close(pipefd[0]);
        if (bytes_read <= 0) {
            return -1;
        }
        geometry[bytes_read] = '\0';  // Null-terminate the string
        if (geometry[bytes_read - 1] == '\n') {
            geometry[bytes_read - 1] = '\0';  // Remove newline if present
        }
        int status;
        waitpid(pid, &status, 0);  // Wait for child process to finish
        return WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 0 : -1;
    }
}

// Execute screenshot command
void execute_screenshot(const char *pictures_dir, const char *filename) {
    char geometry[256];
    if (execute_slurp(geometry, sizeof(geometry)) != 0) {
        fprintf(stderr, "Area selection cancelled or error occurred\n");
        return;
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {  // Child process
        char filepath[PATH_MAX];
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
    sigaction(SIGINT, &sa, NULL);   // Set up signal handler for SIGINT
    sigaction(SIGTERM, &sa, NULL);  // Set up signal handler for SIGTERM

    char *pictures_dir = get_pictures_dir();  // Get pictures directory path
    char *filename = generate_filename();     // Generate unique filename

    if (keep_running) {
        execute_screenshot(pictures_dir, filename);  // Take screenshot
    }

    free(pictures_dir);  // Free allocated memory
    free(filename);      // Free allocated memory

    return 0;
}
