#include "utils.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

bool dir_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0) && S_ISDIR(st.st_mode);
}

void create_dir(const char *path) {
    CHECK(mkdir(path, 0755));
}

char *get_pictures_dir(void) {
    const char *home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "Unable to get home directory\n");
        exit(EXIT_FAILURE);
    }
    char *pictures_dir = malloc(PATH_MAX);
    if (!pictures_dir) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    snprintf(pictures_dir, PATH_MAX, "%s/Pictures", home);
    if (!dir_exists(pictures_dir)) {
        create_dir(pictures_dir);
    }
    return pictures_dir;
}

char *generate_filename(void) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char *filename = malloc(64);
    if (!filename) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strftime(filename, 64, "scr_%Y%m%d_%H%M%S.png", tm);
    return filename;
}
