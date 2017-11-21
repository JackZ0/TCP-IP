#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>
#include <netdb.h>            // struct addrinfo
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t, uint32_t
#include <sys/socket.h>       // needed for socket()
#include <netinet/in.h>       // IPPROTO_ICMP, INET_ADDRSTRLEN
#include <netinet/ip.h>       // struct ip and IP_MAXPACKET (which is 65535)
#include <netinet/ip_icmp.h>  // struct icmp, ICMP_ECHO
#include <arpa/inet.h>        // inet_pton() and inet_ntop()
#include <sys/ioctl.h>        // macro ioctl is defined
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
#include <net/if.h>           // struct ifreq
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)
#include <net/ethernet.h>
#include <errno.h>            // errno, perror()

#define BUFFER_MAX 2048
#define ETH_P_DEAN 2088
#define HELLO_PORT   3456    
#define HELLO_GROUP "224.1.1.1"    
#define MSGBUFSIZE 256    
int main(int argc, char *argv[])    
{    
    struct sockaddr_in addr;    
    int fd, nbytes,addrlen;    
    struct ip_mreq mreq;    
    char msgbuf[MSGBUFSIZE];    
    u_int yes=1;     
        
    if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0)     
    {    
        perror("socket");    
        exit(1);    
    }    
        
        
    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0)     
    {    
        perror("Reusing ADDR failed");    
        exit(1);    
    }    
        
        
    memset(&addr,0,sizeof(addr));    
    addr.sin_family=AF_INET;    
    addr.sin_addr.s_addr=htonl(INADDR_ANY);     
    addr.sin_port=htons(HELLO_PORT);    
        
    if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0)    
    {    
        perror("bind");    
        exit(1);    
    }    
        
    mreq.imr_multiaddr.s_addr=inet_addr(HELLO_GROUP);    
    mreq.imr_interface.s_addr=htonl(INADDR_ANY);    
    if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0)     
    {    
        perror("setsockopt");    
        exit(1);    
    }        
    while (1)     
    {    
        //ssize_t recvfrom(int s, void *buf, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);    
        addrlen=sizeof(addr);    
        if ((nbytes=recvfrom(fd, msgbuf, MSGBUFSIZE, 0, (struct sockaddr *) &addr, (socklen_t *)&addrlen)) < 0)     
        {    
            perror("recvfrom");    
            exit(1);    
        }    
        puts(msgbuf);    
    }    
    return 0;    
}  
