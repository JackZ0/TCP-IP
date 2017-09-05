#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netdb.h>
#include <stddef.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define BUF_SIZE   512
char publicIP[30];             //存储公网IP


//获得公网IP
bool getPublicIP(char *url)
{
    struct sockaddr_in pin;
    struct hostent *nlp_host;
    int sd = 0;
    int len = 0;
    char buf[BUF_SIZE] = { 0 };
    char myurl[100] = {0};
    char host[100] = { 0 };
    char GET[100] =  {0};
    char header[240] =  {0};
    char *pHost = 0;


    ///get the host name and the relative address from url name!!!
    strcpy(myurl, url);
    for (pHost = myurl; *pHost != '/' && *pHost != '\0'; ++pHost) ;
    if ((unsigned int)(pHost - myurl) == strlen(myurl))
        strcpy(GET, "/");
    else
        strcpy(GET, pHost);
    *pHost = '\0';
    strcpy(host, myurl);


    ///setting socket param
    if ((nlp_host = gethostbyname(host)) == 0)
    {
        perror("error get host\n");
        return false;
    }


    bzero(&pin, sizeof(pin));
    pin.sin_family = AF_INET;
    pin.sin_addr.s_addr = htonl(INADDR_ANY);
    pin.sin_addr.s_addr = ((struct in_addr *)(nlp_host->h_addr))->s_addr;
    pin.sin_port = htons(80);


    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error opening socket!!!\n");
        return false;
    }


    ///together the request info that will be sent to web server
    ///Note: the blank and enter key byte is necessary,please remember!!!
    strcat(header, "GET");
    strcat(header, " ");
    strcat(header, GET);
    strcat(header, " ");
    strcat(header, "HTTP/1.1\r\n");
    strcat(header, "HOST:");
    strcat(header, host);
    strcat(header, "\r\n");
    strcat(header, "ACCEPT:*/*");
    strcat(header, "\r\nConnection: close\r\n\r\n\r\n");


    ///connect to the webserver,send the header,and receive the web sourcecode
//    if (::connect(sd, (void *)&pin, sizeof(pin)) == -1)
    if(connect(sd, &pin, sizeof(pin)) == -1)
    {
        close(sd);
        printf("error connect to socket\n");
        return false;
    }




    if (send(sd, header, strlen(header), 0) == -1)
    {
        close(sd);
        perror("error in send \n");
        return false;
    }


    ///send the message and wait the response!!!
    len = recv(sd, buf, BUF_SIZE, 0);
    if (len < 0)
    {
        close(sd);
        printf("receive data error!!!\n");
        return false;
    }
    else
    {
        memset(publicIP,0,sizeof(publicIP));
//buf中存储的数据并不只是公网IP，还有其它数据，需要将公网IP解析出来
//不同的环境中，数据格式可能不同，具体情况具体分析
        sscanf(strstr(buf,"utf-8")+9,"%*[^\n]\n%[^\n]",publicIP);
        printf("ip = %s\n",publicIP);


        close(sd);
        return true;
    }
}


int main(int argc, char *argv[])
{
    char url[] = "http://blog.csdn.net";
    if(getPublicIP(url))
    {
         printf("get public IP.\n");
    }
    else
    {
         printf("fail to get public IP.\n");
    }

    return 0;
}
