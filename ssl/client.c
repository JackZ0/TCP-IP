#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
int main()
{
    BIO *cbio, *out;
    int len;
    char tmpbuf[1024];

    /*建立连接到本地web服务的BIO*/
    cbio = BIO_new_connect("localhost:2323");
    out = BIO_new_fp(stdin, BIO_NOCLOSE);
    
    /*发出连接请求*/
    if(BIO_do_connect(cbio) <= 0)
    {
        fprintf(stderr, "Error connecting to server\n");
    }
    
    /*发送消息*/
    BIO_puts(cbio, "GET / HTTP/1.0\n\n");
    while(1)
    {    
        /*接收输入*/
        memset(tmpbuf,0,1024);
        scanf("%s",&tmpbuf);
        len=strlen(tmpbuf);
        BIO_write(out, tmpbuf, len);
        
        /*发送*/
        len = BIO_write(cbio, tmpbuf, len);
        if(len <= 0 || tmpbuf[0]=='q') 
            break;
    }
    
    /*释放资源*/
    BIO_free(cbio);
    BIO_free(out);
    return 0;
}
