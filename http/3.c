#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>

#define SA      struct sockaddr
#define MAXLINE 4096
#define MAXSUB  200


#define LISTENQ         1024

extern int h_errno;

ssize_t process_http(int sockfd, char *host, char *page, char *poststr)
{
	char sendline[MAXLINE + 1], recvline[MAXLINE + 1];
	ssize_t n;
	snprintf(sendline, MAXSUB,
                 "HOST: %s\r\n"
                 "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:56.0) Gecko/20100101 Firefox/56.0\r\n"
                 "Accep:*/*\r\n"
                 "Accept-Language:zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3\r\n"
                 "Accept-Encoding:gzip, deflate, br\r\n"
                 "Connection: keep-alive\r\n"
		 "%s", page, host, strlen(poststr), poststr);

	write(sockfd, sendline, strlen(sendline));
	while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = '\0';
		printf("%s", recvline);
	}
	return n;

}
int main(void)
{
	int sockfd;
	struct sockaddr_in servaddr;

	char **pptr;
	//********** You can change. Puy any values here *******
	char *hname = "vdi.talengain.com";
	char *page = "/vpn/index.html";
	char *poststr = "mode=login&user=test&password=test\r\n";
	//*******************************************************

	char str[50];
	struct hostent *hptr;
	if ((hptr = gethostbyname(hname)) == NULL) {
		fprintf(stderr, " gethostbyname error for host: %s: %s",
			hname, hstrerror(h_errno));
		exit(1);
	}
	printf("hostname: %s\n", hptr->h_name);
	if (hptr->h_addrtype == AF_INET
	    && (pptr = hptr->h_addr_list) != NULL) {
		printf("address: %s\n",
		       inet_ntop(hptr->h_addrtype, *pptr, str,
				 sizeof(str)));
	} else {
		fprintf(stderr, "Error call inet_ntop \n");
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(38251);
	inet_pton(AF_INET, str, &servaddr.sin_addr);

	connect(sockfd, (SA *) & servaddr, sizeof(servaddr));
	process_http(sockfd, hname, page, poststr);
	close(sockfd);
	exit(0);

}
