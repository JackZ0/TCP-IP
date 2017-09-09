#include <unistd.h>  
#include <stdio.h>  
#include <errno.h>  
int main()  
{  
    pid_t fpid;  
    int count =0;  
    fpid = fork();  
    if( fpid < 0)   
    {  
        printf("\r\n error");  
    }  
    else if( 0 == fpid)  
    {  
        printf("I am child. my pid=%ld parent pid=%ld \r\n", (long)getpid(),(long)getppid());  
        exit(1);  
    }  
    else  
    {  
         int exitcode = 0;  
         long  ret;  
         sleep(1);  
         printf("I am parent. my pid=%d  \r\n", getpid());  
         printf(" wait for child to exit ... \r\n");  
         ret = wait(&exitcode);  
         while( -1 != ret )  
         {  
                printf("child pid = %ld exit code=%d \r\n", ret, exitcode);  
                if(errno == ECHILD) break;  
                   sleep(1);  
                ret = wait(&exitcode);  
         }  
         printf(" success for wait for child to exit. \r\n");  
    }  
    return 0;  
}
