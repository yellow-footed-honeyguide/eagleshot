#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

#define CHECK(call)             \
    do {                        \
        if ((call) == -1) {     \
            perror(#call);      \
            exit(EXIT_FAILURE); \
        }                       \
    } while (0)

bool dir_exists(const char *path);
void create_dir(const char *path);
char *get_pictures_dir(void);
char *generate_filename(void);

#endif // UTILS_H
