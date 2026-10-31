//classic cd command for changing directories.

#include <stdio.h>
#include <unistd.h>

void change_directory(const char *path) {
    if (path == NULL) {
        printf("cd: missing argument\n");
        return;
    }

    if (chdir(path) == 0) {
        char cwd[256];
        getcwd(cwd, sizeof(cwd));
        printf("Current directory: %s\n", cwd);
    } else {
        perror("cd");
    }
}