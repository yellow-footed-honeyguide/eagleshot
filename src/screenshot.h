#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <stddef.h>

int execute_slurp(char *geometry, size_t size);
void execute_screenshot(const char *pictures_dir, const char *filename);

#endif // SCREENSHOT_H
