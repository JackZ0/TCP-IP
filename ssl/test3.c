#include <openssl/crypto.h>
#include <openssl/bio.h>

int main()
{
    char *p;
    BIO *b;
    CRYPTO_malloc_debug_init();
    CRYPTO_set_mem_debug_options(V_CRYPTO_MDEBUG_ALL);
    CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON); /*开户内存记录*/
    
    p=OPENSSL_malloc(4);
    CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_OFF);/*关闭内存记录*/
    b=BIO_new_file("leak.log","w");
    CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);
    CRYPTO_mem_leaks(b);    /*将内存泄露输出到FILE中*/
    
    OPENSSL_free(p);
    BIO_free(b);
    return 0;
}
