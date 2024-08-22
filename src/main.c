#include "config.h"
#include "screenshot.h"
#include "utils.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

volatile sig_atomic_t keep_running = 1;

void signal_handler(int sig) {
    (void)sig;  // Подавляем предупреждение о неиспользуемом параметре
    keep_running = 0;
}

int main(int argc, char *argv[]) {
    if (argc > 1 && (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)) {
        printf("eagleshot version %s\n", VERSION);
        return 0;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    char *pictures_dir = get_pictures_dir();
    char *filename = generate_filename();

    if (keep_running) {
        execute_screenshot(pictures_dir, filename);
    }

    free(pictures_dir);
    free(filename);

    return 0;
}
