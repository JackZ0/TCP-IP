#include <stdio.h>
#include <openssl/bio.h>

int main()
{
    BIO *b=NULL;
    int len=0,outlen=0;
    char *out=NULL;

    /*创建文件，写入内容*/
    b=BIO_new_file("bf.txt","w");
    len=BIO_write(b,"hello",5);
    len=BIO_printf(b,"%s"," world");
    BIO_free(b);
    
    /*读取文件内容*/
    b=BIO_new_file("bf.txt","r");
    len=BIO_pending(b);
    len=50;
    out=(char *)OPENSSL_malloc(len);
    len=1;
    while(len>0)
    {
        len=BIO_read(b,out+outlen,1);
        outlen+=len;
    }
    
    /*打印读取内容*/
    printf("%s\n",out);
    
    /*释放资源*/
    BIO_free(b);
    free(out);
    return 0;
}
