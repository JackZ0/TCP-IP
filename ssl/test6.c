#include <stdio.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

int main()
{
    BIO *bmd=NULL,*b=NULL;
    const EVP_MD *md=EVP_md5();
    int len;
    char tmp[1024];

    /*创建一个md BIO*/
    bmd=BIO_new(BIO_f_md());
    
    /*设置md BIO 为md5 BIO*/
    BIO_set_md(bmd,md);
    
    /*创建一个null BIO*/
    b= BIO_new(BIO_s_null());
    
    /*构造BIO链,md5 BIO在顶部*/
    b=BIO_push(bmd,b);
    
    /*将字符串送入BIO做摘要*/
    len=BIO_write(b,"openssl",7);
    
    /*将摘要结果写入tmp缓冲区并输出*/
    len=BIO_gets(b,tmp,1024);
    puts(tmp);
    
    BIO_free(b);
    return 0;
}
