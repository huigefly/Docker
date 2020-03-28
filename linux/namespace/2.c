#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


#define STACK_SIZE (1024 * 1024)

static char child_stack[STACK_SIZE];
char * const child_args[] = {
    "/bin/bash",
    NULL
};

static int mount_filesystem()
{
    int ret; 
    ret = mount ("none", "/proc", "proc", 0, NULL);
    if (ret < 0){
        printf("mount failed!\n");
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

int main()
{
    printf ("parent begin!\n");
    int child_pid = clone(
        child_main,
        child_stack + STACK_SIZE,
        CLONE_NEWPID|CLONE_NEWNS|CLONE_NEWUTS|SIGCHLD,
        NULL);
    waitpid(child_pid, NULL, 0);
    printf("quited\n");

    return 0;
}