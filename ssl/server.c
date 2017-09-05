#include <stdio.h>
#include <openssl/bio.h>
#include <string.h>

int main()
{
    BIO *b=NULL,*c=NULL;
    int sock,ret,len;
    char *addr=NULL;
    char out[80];

    /*设定端口*/
    sock=BIO_get_accept_socket("2323",0);
    
    /*建立服务器的BIO*/
    b=BIO_new_socket(sock, BIO_NOCLOSE);
    ret=BIO_accept(sock,&addr);
    
    /*建立输出到屏幕的BIO*/
    BIO_set_fd(b,ret,BIO_NOCLOSE);
    
    /*读取并输出*/
    while(1)
    {
        memset(out,0,80);
        len=BIO_read(b,out,80);
        if(out[0]=='q')
            break;
        printf("%s\n",out);
    }
    
    /*释放资源*/
    BIO_free(b);
    return 0;
}
