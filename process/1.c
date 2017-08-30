#include <stdlib.h>
#include <unistd.h>
void main(){
    int i;
    if (fork() == 0) {
        /* 子进程程序 */
        for (i = 1; i <100; i++)
            printf("This is child process\n");
    }
    else {
        /* 父进程程序*/
        for (i = 1; i <100; i++)
            printf("This is parent process\n");
    }
}
