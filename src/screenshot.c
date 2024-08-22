#include "screenshot.h"
#include "utils.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


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
    } else if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execlp("slurp", "slurp", NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        close(pipefd[1]);
        ssize_t bytes_read = read(pipefd[0], geometry, size - 1);
        close(pipefd[0]);
        
        if (bytes_read <= 0) {
            return -1;
        }
        
        geometry[bytes_read] = '\0';
        if (geometry[bytes_read - 1] == '\n') {
            geometry[bytes_read - 1] = '\0';
        }
        
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 0 : -1;
    }
}

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
    } else if (pid == 0) {
        char filepath[PATH_MAX];
        snprintf(filepath, PATH_MAX, "%s/%s", pictures_dir, filename);
        execlp("grim", "grim", "-g", geometry, filepath, NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Error executing grim\n");
        } else {
            printf("Screenshot saved: %s/%s\n", pictures_dir, filename);
        }
    }
}
