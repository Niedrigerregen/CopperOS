#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "cd.h"

#define MAX_CMD 256
#define ANSI_ORNG "\033[38;5;202m" // turns the Logo orange because it looks cooler. I was lying. It turns everything orange. Not planning on fixing that

   void print_logo_content(void) {

    printf("\n");
    printf( ANSI_ORNG"   /$$$$$$                                                             /$$$$$$   /$$$$$$ \n");
    printf( ANSI_ORNG"  /$$__  $$                                                           /$$__  $$ /$$__  $$\n");
    printf( ANSI_ORNG" | $$  \\__/  /$$$$$$   /$$$$$$   /$$$$$$   /$$$$$$   /$$$$$$         | $$  \\ $$| $$  \\__/\n");
    printf( ANSI_ORNG" | $$       /$$__  $$ /$$__  $$ /$$__  $$ /$$__  $$ /$$__  $$ /$$$$$$| $$  | $$|  $$$$$$ \n");
    printf( ANSI_ORNG" | $$      | $$  \\ $$| $$  \\ $$| $$  \\ $$| $$$$$$$$| $$  \\__/|______/| $$  | $$ \\____  $$\n");
    printf( ANSI_ORNG" | $$    $$| $$  | $$| $$  | $$| $$  | $$| $$_____/| $$              | $$  | $$ /$$  \\ $$\n");
    printf( ANSI_ORNG" |  $$$$$$/|  $$$$$$/| $$$$$$$/| $$$$$$$/|  $$$$$$$| $$              |  $$$$$$/|  $$$$$$/\n");
    printf( ANSI_ORNG"  \\______/  \\______/ | $$____/ | $$____/  \\_______/|__/               \\______/  \\______/ \n");
    printf( ANSI_ORNG"                     | $$      | $$                                                       \n");
    printf( ANSI_ORNG"                     | $$      | $$                                                       \n");
    printf( ANSI_ORNG"                     |__/      |__/                                                       \n");
    printf("\n");
}

    void print_logo(void) {

    static int firstTime = 1; // One of the most primitive bugfixing methods there is: A counter. I'm very proud of that

     if (firstTime) {
        firstTime = 0;
        print_logo_content();            
    } 
}

void print_logo_force(void) {

    print_logo_content();
}

int main() {
    char command[MAX_CMD];

    print_logo();
    
    while (1) {
        printf("$> ");
        if (!fgets(command, sizeof(command), stdin))
            continue;

        
        command[strcspn(command, "\n")] = 0;

        
        if (strncmp(command, "cd ", 3) == 0) {
            change_directory(command + 3);
            continue;
        }

        
        if (strcmp(command, "clear") == 0) {
            pid_t pid = fork();
            if (pid == 0) {
                execl("/bin/clear", "clear", NULL);
                perror("exec failed");
                _exit(1);
            } else if (pid > 0) {
                wait(NULL);
                print_logo_force();
            } else {
                perror("fork failed");
            }
            continue;
        }

        if (strcmp(command, "help") == 0) {
    printf("Available commands:\n");
    printf("help - Show this cool help message\n");
    printf("---system---\n");
    printf("cd - Change directory\n"); // not really effective in a live OS but it can stay for when i turn it into an ... on demand OS?
    printf("ls - List files in the current directory\n");
    printf("list - List files in the current directory\n");
    printf("shutdown - Shutdown the system\n");
    printf("reboot - reboots your system\n");
    printf("clear - Clear the terminal screen\n");
    printf("echo - Echoes the input back to the terminal\n");
    printf("---utilities---\n");
    printf("math - Perform basic math operations\n");
    printf("fortune - shows you a random quote from the movie Fight Club\n");
    printf("date - shows you the current date and time\n");
    printf("tellmeafact - tells you exactly one fact");
    printf("---internet---\n");
    printf("search - search the web with a URL or an Alias\n");
    printf("---package manager---\n");
    printf("cop [package] - Installs a package from the Copper_DB repo\n");
    printf("more commands in the future...\n");
    continue;
} 

char *args[MAX_CMD];
char cmd_copy[MAX_CMD];
strncpy(cmd_copy, command, MAX_CMD - 1);
cmd_copy[MAX_CMD - 1] = '\0';

int i = 0;
args[i] = strtok(cmd_copy, " ");
while (args[i] != NULL) {
    i++;
    args[i] = strtok(NULL, " ");
}

char bin_path[MAX_CMD];
snprintf(bin_path, sizeof(bin_path), "/bin/%s", args[0]);
args[0] = bin_path;

pid_t pid = fork();
if (pid == 0) {
    execvp(bin_path, args);
    perror("exec failed");
    _exit(1);
} else if (pid > 0) {
    wait(NULL);
} else {
    perror("fork failed");
}
    }

    return 0;
}