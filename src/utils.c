#include <errno.h>     // errno (variable), used for error reporting
#include <limits.h>    // PATH_MAX (macro), maximum length of a file path
#include <stdio.h>     // fprintf, snprintf (functions) for formatted output
#include <stdlib.h>    // getenv, malloc, exit, EXIT_FAILURE (macro)
#include <string.h>    // (potentially used string manipulation functions)
#include <sys/stat.h>  // struct stat (type), stat (function), S_ISDIR (macro)
#include <time.h>      // time, localtime (functions), struct tm (type)
#include <unistd.h>    // (potentially used POSIX operating system API functions)

#include "utils.h"  // dir_exists, create_dir, get_pictures_dir, generate_filename (functions)

// Function to check if a directory exists at the given path
bool dir_exists(const char *path) {
    struct stat st;  // Structure to store file/directory information
    // stat() gets info about the file/directory, S_ISDIR checks if it's a directory
    return (stat(path, &st) == 0) && S_ISDIR(st.st_mode);
}

// Function to create a directory with read/write/execute permissions (0755)
void create_dir(const char *path) {
    CHECK(mkdir(path, 0755));  // Create directory, CHECK macro handles errors
}

// Function to get or create the Pictures directory in the user's home folder
char *get_pictures_dir(void) {
    const char *home = getenv("HOME");  // Get the path to the user's home directory
    if (!home) {                        // If HOME environment variable is not set
        fprintf(stderr, "Unable to get home directory\n");  // Print error message
        exit(EXIT_FAILURE);                                 // Exit the program with failure status
    }
    char *pictures_dir = malloc(PATH_MAX);  // Allocate memory for the Pictures directory path
    if (!pictures_dir) {                    // If memory allocation failed
        perror("malloc");                   // Print error message
        exit(EXIT_FAILURE);                 // Exit the program with failure status
    }
    // Construct the path to the Pictures directory
    snprintf(pictures_dir, PATH_MAX, "%s/Pictures", home);
    if (!dir_exists(pictures_dir)) {  // If the Pictures directory doesn't exist
        create_dir(pictures_dir);     // Create it
    }
    return pictures_dir;  // Return the path to the Pictures directory
}

// Function to generate a unique filename based on current date and time
char *generate_filename(void) {
    time_t t = time(NULL);          // Get current time
    struct tm *tm = localtime(&t);  // Convert time to local time structure
    char *filename = malloc(64);    // Allocate memory for the filename
    if (!filename) {                // If memory allocation failed
        perror("malloc");           // Print error message
        exit(EXIT_FAILURE);         // Exit the program with failure status
    }
    // Format the filename with current date and time
    strftime(filename, 64, "scr_%Y%m%d_%H%M%S.png", tm);
    return filename;  // Return the generated filename
}
