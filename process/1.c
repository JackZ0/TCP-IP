#include <stdlib.h>
#include <unistd.h>
void main(){
    int i;
    int fd =0;
    fd = fork();
    printf("fd = %d \n",fd);
    if (fd== 0) 
    {
        /* 子进程程序 */
        for (i = 1; i <10; i++)
            printf(" %d This is child process 子进程程序 \n",i);
    }
    else 
    {
        /* 父进程程序*/
        for (i = 1; i <10; i++)
            printf("%d This is parent process 父进程程序 \n",i);
    }
}
