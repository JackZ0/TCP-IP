#include <stdio.h>
#include <openssl/bio.h>

int main()
{
    BIO *b=NULL;
    int len=0;
    char *out=NULL;

    /*mem类型的BIO*/ 
    b=BIO_new(BIO_s_mem());  
    
    /*写入内容*/ 
    len=BIO_write(b,"openssl",7); 
    len=BIO_printf(b,"%s","bio test");
    
    /*得到缓冲区中待读取大小*/ 
    len=BIO_ctrl_pending(b);  
    out=(char *)OPENSSL_malloc(len);
    
    /*读取内容并打印*/
    len=BIO_read(b,out,len);
    printf("%s\n", out);

    /*释放资源*/
    OPENSSL_free(out);
    BIO_free(b);
    return 0;
}
