#include <curl/curl.h>
#include <stdio.h>
#include <string.h>

CURL *curl;
CURLcode res;

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    if (strlen((char *)stream) + strlen((char *)ptr) > 999999) return 0;
      strcat(stream, (char *)ptr);
    return size*nmemb;
}

char *down_file(char *filename)
{
    static char str[10000000];
    strcpy(str, "");
    //return “”;

    curl_easy_setopt(curl, CURLOPT_URL, filename); //设置下载地址

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);//设置超时时间

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);//设置写数据的函数

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, str);//设置写数据的变量

    res = curl_easy_perform(curl);//执行下载

    str[9999999] = '\0';
    if(CURLE_OK != res) return NULL;//判断是否下载成功

    return str;
}

int main()
{
    char url[200];
    curl = curl_easy_init();//对curl进行初始化

    char *result;
    while(fgets(url, 200, stdin)){
   result = down_file(url);
   if (result) puts(result);
   else puts("Get Error!");
   printf("\nPlease Input a url:");

}
curl_easy_cleanup(curl);//释放curl资源


return 0;
}

