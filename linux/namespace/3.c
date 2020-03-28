#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define STACK_SIZE (1024 * 1024)

char *rootDir = NULL;
static char child_stack[STACK_SIZE];
char * const child_args[] = {
    "/bin/bash",
    NULL
};

int change_root()
{
    if (rootDir){
        int rootfd;
        rootfd = open(rootDir, O_RDONLY);
        if (rootfd < 0){
            printf("open rootdir failed\n");
            return 1;
        }
        fchdir(rootfd);
        chroot(".");
        close(rootfd);
        printf("chroot %s ok\n", rootDir);
    }
    return 0;
}

static int mount_filesystem()
{
    int ret; 
    ret = mount ("none", "/proc", "proc", 0, NULL);
    if (ret < 0){
        printf("mount proc failed!\n");
        return -1;
    }
    ret = mount ("none", "/sys", "sysfs", 0, NULL);
    if (ret < 0){
        printf("mount sys failed!, %d\n", errno);
        return -1;
    }
    return 0;
}

int child_main(void *args){
    int ret;
    printf("child begin!\n");
    sethostname("zhengfh", 7);
    mount_filesystem();
    execv(child_args[0], child_args);
    // int nCount = 5;
    // while (nCount--){
    //     printf("count:%d\n", nCount);
    //     sleep(1);
    // }
    return 1;
}

int main(int argc, char *argv[])
{
    rootDir = argv[1];
    printf ("parent begin:%s!\n", rootDir);
    int child_pid = clone(
        child_main,
        child_stack + STACK_SIZE,
        CLONE_NEWPID|CLONE_NEWNS|CLONE_NEWUTS|SIGCHLD,
        NULL);
    waitpid(child_pid, NULL, 0);
    printf("quited\n");

    return 0;
}
