#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "share.h"


int main()
{
 int running = 1; //程序运行标志位
 void *shared_memory = (void *)0;
 struct shared_use_st *shared_stuff;
 char buffer[BUFSIZ];
 int shmid; //共享内存标识符

 

 /*创建共享内存*/
 shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
 if (shmid == -1) 
 {
  fprintf(stderr, "shmget failed\n");
  exit(EXIT_FAILURE);
 }

 

 /*将共享内存连接到一个进程的地址空间中*/
 shared_memory = shmat(shmid, (void *)0, 0);//指向共享内存第一个字节的指针
 if (shared_memory == (void *)-1) 
 {
  fprintf(stderr, "shmat failed\n");
  exit(EXIT_FAILURE);
 }

 printf("Memory attached at %X\n", (int)shared_memory);
 shared_stuff = (struct shared_use_st *)shared_memory;

 

 /*生产者写入数据*/
 while(running) 
 {
  while(shared_stuff->written_by_you == 1) 
  {
   sleep(1);            
   printf("waiting for client...\n");
  }
  printf("Enter some text: ");
  fgets(buffer, BUFSIZ, stdin);
  strncpy(shared_stuff->some_text, buffer, TEXT_SZ);
  shared_stuff->written_by_you = 1;
  if (strncmp(buffer, "end", 3) == 0) 
  {
   running = 0;
  }
 }

 

    /*该函数用来将共享内存从当前进程中分离,仅使得当前进程不再能使用该共享内存*/
 if (shmdt(shared_memory) == -1) 
 {
  fprintf(stderr, "shmdt failed\n");
  exit(EXIT_FAILURE);
 }

 printf("producer exit.\n");
 exit(EXIT_SUCCESS);
}
