#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "config.h"

#define MAX_CMD_LENGTH 256

// Function to check if a directory exists
int dir_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0) && S_ISDIR(st.st_mode);
}

// Function to create a directory
int create_dir(const char *path) {
    return mkdir(path, 0755);
}

int main(int argc, char *argv[]) {
    // Check if the program was called with --version
    if (argc > 1 && (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)) {
        printf("eagleshot version %s\n", VERSION);
        return 0;
    }

    // Get the home directory
    char *home = getenv("HOME");
    if (home == NULL) {
        fprintf(stderr, "Unable to get HOME directory\n");
        return 1;
    }

    // Construct the Pictures directory path
    char pictures_dir[MAX_CMD_LENGTH];
    snprintf(pictures_dir, sizeof(pictures_dir), "%s/Pictures", home);

    // Check if Pictures directory exists, create if it doesn't
    if (!dir_exists(pictures_dir)) {
        if (create_dir(pictures_dir) != 0) {
            fprintf(stderr, "Failed to create Pictures directory\n");
            return 1;
        }
    }

    // Get current time for filename
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char filename[64];
    strftime(filename, sizeof(filename), "screenshot_%Y-%m-%d_%H-%M-%S.png", tm);

    // Construct the full command
    char command[MAX_CMD_LENGTH];
    snprintf(command, sizeof(command),
             "grim -g \"$(slurp)\" \"%s/%s\"",
             pictures_dir, filename);

    // Execute the command
    return system(command);
}
