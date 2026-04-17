// Init for CopperOS. Starts basically everything
#include <unistd.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main() {

    // Set PATH so BusyBox applets can be found
    setenv("PATH", "/bin:/sbin:/usr/bin", 1);

    // Remount root as read-write
    mount("proc", "/proc", "proc", 0, NULL);
    if (mount("/dev/root", "/", NULL, MS_REMOUNT, NULL) != 0) {
        if (mount("", "/", NULL, MS_REMOUNT, NULL) != 0) {
            perror("remount / rw failed (ignoring)");
        }
    }

    sethostname("copperos", 8);

    system("ifconfig lo up");
    
    // Configure ethernet with static IP using ifconfig
    system("ifconfig eth0 up");
    system("ifconfig eth0 10.0.2.15 netmask 255.255.255.0"); //hey! this one actually works without ethernet and i don't know why! No really i don't know how and i'm scared
    system("route add default gw 10.0.2.2 eth0"); // i found out now. QEMU is just giving out it's own IP so nothing of this would actually work on hardware but idc
    
    // testing with google's DNS but i forgot to change it back but i don't really care
    FILE *f = fopen("/etc/resolv.conf","w");
    if(f) {
        fprintf(f,"nameserver 8.8.8.8\n"); 
        fprintf(f,"nameserver 1.1.1.1\n");
        fclose(f);
    }

    mount("devtmpfs", "/dev",  "devtmpfs", 0, NULL); // Mount folders
    mount("proc",     "/proc", "proc",     0, NULL);
    mount("sysfs",    "/sys",  "sysfs",    0, NULL);
    mount("tmpfs",    "/tmp",  "tmpfs",    0, NULL);
    
    int fd = open("/dev/console", O_RDWR);
    dup2(fd, 0); // stdin
    dup2(fd, 1); // stdout
    dup2(fd, 2); // stderr
    if (fd > 2) close(fd);

    setenv("LD_LIBRARY_PATH", "/lib", 1); // Set library path for dynamic linking
    setenv("TERM", "ansi", 1); // Set terminal type for compatibility with apps
    execl("/bin/shell", "shell", NULL); // Start the shell

    
    write(2, "execl failed!\n", 14);
    while(1);
}

/* Won't reboot after a failure because 
* a fail is most of a time an error by 
* me in the code so it's a loop 
* one way or another
* well okay also if shell dies, everything dies but something
* like this doesn't happen in Qemu so i don't care
*/