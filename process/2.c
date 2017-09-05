#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
void mydaemon()
{
    int i,fd0;
    pid_t pid;
    struct sigaction sa;
    umask(0);
    if(pid=fork()>0)  //创建子进程，让父进程退出
        return;
    setsid();         //创建会话
    sa.sa_flags=0;
    if(sigaction(SIGCHLD,&sa,NULL<0))
        return;
    if(pid=fork()>0)        //第二次fork
        return;
    if(chdir("/")<0)
        return ;
    close(0);
    fd0=open("/dev/null",O_RDWR);
    dup2(fd0,1);
    dup2(fd0,2);
}
int main()
{
   mydaemon();

   while(1)
       sleep(1);
   return 0;
}
