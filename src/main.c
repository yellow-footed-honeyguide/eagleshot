#include <signal.h>  // sigaction (function), sig_atomic_t (type), SIGINT (macro), SIGTERM (macro)
#include <stdio.h>   // printf (function)
#include <stdlib.h>  // free (function)
#include <string.h>  // strcmp (function), memset (function)

#include "config.h"      // VERSION (macro)
#include "screenshot.h"  // execute_screenshot (function)
#include "utils.h"       // get_pictures_dir, generate_filename (functions)

// Declare a global variable to control program execution
// volatile is used to prevent compiler optimization
// sig_atomic_t guarantees atomic operations with this variable
volatile sig_atomic_t keep_running = 1;

// Define the signal handler function
void signal_handler(int sig) {
    // Suppress the warning about unused parameter
    // Casting to void tells the compiler we intentionally ignore the parameter
    (void)sig;
    // Set the flag to terminate the program
    keep_running = 0;
}

// Main function of the program
int main(int argc, char *argv[]) {
    // Check if an argument for displaying the program version is passed
    if (argc > 1 && (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)) {
        // If yes, print the version and exit the program
        printf("eagleshot version %s\n", VERSION);
        return 0;
    }

    // Create a structure for configuring the signal handler
    struct sigaction sa;
    // Initialize the structure with zeros
    memset(&sa, 0, sizeof(sa));
    // Set our handler function
    sa.sa_handler = signal_handler;
    // Register the handler for SIGINT signal (Ctrl+C)
    sigaction(SIGINT, &sa, NULL);
    // Register the handler for SIGTERM signal
    sigaction(SIGTERM, &sa, NULL);

    // Get the path to the directory for saving screenshots
    char *pictures_dir = get_pictures_dir();
    // Generate a unique filename for the screenshot
    char *filename = generate_filename();

    // Check if no signal to terminate has been received
    if (keep_running) {
        // If not, perform screenshot creation
        execute_screenshot(pictures_dir, filename);
    }

    // Free the memory allocated for the directory path
    free(pictures_dir);
    // Free the memory allocated for the filename
    free(filename);

    // Exit the program with a successful execution code
    return 0;
}
