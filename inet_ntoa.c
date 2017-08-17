#include <stdio.h>   
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h>   
#include <string.h>   
int main(int argc, char *argv[])   
{   
    struct in_addr addr1,addr2;   
    ulong l1,l2;   
    l1= inet_addr("0.0.0.0");   
    l2 = inet_addr("211.100.21.179");   
    memcpy(&addr1, &l1, 4);   
    memcpy(&addr2, &l2, 4);   
    printf("%s : %s\n", inet_ntoa(addr1), inet_ntoa(addr2)); //注意这一句的运行结果   
    printf("%s\n", inet_ntoa(addr1));   
    printf("%s\n", inet_ntoa(addr2));   
    return 0;   
}   
