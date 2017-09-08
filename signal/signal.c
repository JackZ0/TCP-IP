#include <signal.h>
#include <stdio.h>
#include <unistd.h>
/*捕捉到信号sig之后，执行预先预定的动作函数*/
void sig_alarm(int sig)
{
    printf("---the signal received is %d. /n", sig);
    signal(SIGINT, SIG_DFL); //SIGINT终端中断信号，SIG_DFL：恢复默认行为，SIN_IGN：忽略信号
}

int main()
{
    signal(SIGINT, sig_alarm);//捕捉终端中断信号

    while(1) 
   {
       printf("waiting here!/n");
       sleep(1);
   }
   return 0;
}
