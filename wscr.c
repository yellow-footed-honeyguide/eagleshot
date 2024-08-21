#include <stdio.h>   // Include for printf
#include <stdlib.h>  // Include the standard library for system() function
#include <string.h>  // Include for strcmp

#include "config.h"  // Include the generated config file

int main(int argc, char *argv[]) {
    // Check if the program was called with --version
    if (argc > 1 && (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)) {
        printf("wscr version %s\n", VERSION);
        return 0;
    }

    // Execute a shell command to take a screenshot
    return system(
        "grim"                                // Use grim (screenshot tool for Wayland)
        " -g \"$(slurp)\""                    // -g option with slurp for area selection
        " \"$HOME/Pictures/screenshot.png\""  // Save to Pictures folder
    );
}
