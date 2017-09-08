#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MAX_TEXT 512
/*用于消息收发的结构体--my_msg_type：消息类型，some_text：消息正文*/
struct my_msg_st
{
    long int my_msg_type;
    char some_text[MAX_TEXT];
};

 

int main()
{
    int running = 1;//程序运行标识符
    struct my_msg_st some_data;
    int msgid;//消息队列标识符
    char buffer[BUFSIZ];

 

 /*创建与接受者相同的消息队列*/
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1) 
    {
        fprintf(stderr, "msgget failed with error: %d/n", errno);
        exit(EXIT_FAILURE);
    }

 /*向消息队列中发送消息*/
    while(running) 
    {
        printf("Enter some text: ");
        fgets(buffer, BUFSIZ, stdin);
        some_data.my_msg_type = 1;
        strcpy(some_data.some_text, buffer);
        if (msgsnd(msgid, (void *)&some_data, MAX_TEXT, 0) == -1) 
        {
             fprintf(stderr, "msgsnd failed/n");
             exit(EXIT_FAILURE);
        }
        if (strncmp(buffer, "end", 3) == 0) 
        {
             running = 0;
        }
    }
    exit(EXIT_SUCCESS);
}
