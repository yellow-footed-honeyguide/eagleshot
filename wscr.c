#include <stdlib.h>  // Include the standard library for system() function

int main() {
    // Execute a shell command to take a screenshot
    return system(
        "grim"                                // Use grim (screenshot tool for Wayland)
        " -g \"$(slurp)\""                    // -g option with slurp for area selection
        " \"$HOME/Pictures/screenshot.png\""  // Save to Pictures folder
    );
}
