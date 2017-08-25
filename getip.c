#include <stdio.h>        
#include <sys/types.h>  
#include <ifaddrs.h>  
#include <netinet/in.h>   
#include <string.h>   
#include <arpa/inet.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <string.h>  
#include <stdlib.h>  
#include <stdlib.h>  
#include <time.h>  
#include <sys/vfs.h>  
#include <stdio.h>  
#include <signal.h>  
#include <sys/stat.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>  
  
  
int main (int argc, const char * argv[])   
{  
  
    struct ifaddrs * ifAddrStruct=NULL;  
    void * tmpAddrPtr=NULL;  
  
    getifaddrs(&ifAddrStruct);  
  
    while (ifAddrStruct!=NULL)   
    {  
        if (ifAddrStruct->ifa_addr->sa_family==AF_INET)  
        {   
            tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;  
            char addressBuffer[INET_ADDRSTRLEN];  
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);  
            printf("%s IPV4 Address %s\n", ifAddrStruct->ifa_name, addressBuffer);   
        }  
        else if (ifAddrStruct->ifa_addr->sa_family==AF_INET6)  
        {
            tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;  
            char addressBuffer[INET6_ADDRSTRLEN];  
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);  
            printf("%s IPV6 Address %s\n", ifAddrStruct->ifa_name, addressBuffer);   
        }   
        ifAddrStruct = ifAddrStruct->ifa_next;  
    }  
    return 0;  
}  

