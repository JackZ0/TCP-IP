#include <stdio.h>
#include <unistd.h>

#define BUFFER_SIZE 30
int main()
{
    int x;
    int fd[2];
    char buf[BUFFER_SIZE];
    char s[BUFFER_SIZE];
    pipe(fd);//创建管道

    while((x=fork())==-1);//创建管道失败时，进入循环
 
     /*进入子进程，子进程向管道中写入一个字符串*/
    if(x==0)
    {
        sprintf(buf,"This is an example of pipe!\n");
        write(fd[1],buf,BUFFER_SIZE);
        exit(0);
    }
    /*进入父进程，父进程从管道的另一端读出刚才写入的字符串*/
    else
    {
        wait(0);//等待子进程结束
        read(fd[0],s,BUFFER_SIZE);//读出字符串，并将其储存在char s[]中
        printf("%s",s);//打印字符串
    }
    return 0;
}
