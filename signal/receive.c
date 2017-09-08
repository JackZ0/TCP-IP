#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/*用于消息收发的结构体--my_msg_type：消息类型，some_text：消息正文*/
struct my_msg_st
{
    long int my_msg_type;
    char some_text[BUFSIZ];
};

 

int main()
{
    int running = 1;//程序运行标识符
    int msgid; //消息队列标识符
    struct my_msg_st some_data; 
    long int msg_to_receive = 0;//接收消息的类型--0表示msgid队列上的第一个消息

 

 /*创建消息队列*/
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1) 
    {
         fprintf(stderr, "msgget failed with error: %d/n", errno);
         exit(EXIT_FAILURE);
    }

 

 /*接收消息*/
    while(running) 
    {
        if (msgrcv(msgid, (void *)&some_data, BUFSIZ,msg_to_receive, 0) == -1) 
        {
             fprintf(stderr, "msgrcv failed with error: %d/n", errno);
             exit(EXIT_FAILURE);
        }
        printf("You wrote: %s", some_data.some_text);
        if (strncmp(some_data.some_text, "end", 3) == 0) 
        {
             running = 0;
        }
    }

 

 /*删除消息队列*/
    if (msgctl(msgid, IPC_RMID, 0) == -1) 
    {
        fprintf(stderr, "msgctl(IPC_RMID) failed/n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
