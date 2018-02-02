#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
size_t function( void *ptr, size_t size, size_t nmemb, void *stream)
{
    printf("%s\n", (char*)ptr);
    return 0;
}

int main(int argc,char *argv[])
{
    CURL *curl;
    int i = 100;
    struct curl_slist *cookies = NULL;
    struct curl_slist *cookie = NULL;
    struct curl_slist *headers = NULL;

    curl = curl_easy_init(); //初始化

    //http请求头
    headers = curl_slist_append(headers,"GET http://ddc.zhangh.com  HTTP/1.1");
    headers = curl_slist_append(headers,"Host:ddc.zhangh.com");
    headers = curl_slist_append(headers,"Connection: keep-alive");
    headers = curl_slist_append(headers,"User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.104 Safari/537.36 Core/1.53.2141.400 QQBrowser/9.5.10219.400");
    headers = curl_slist_append(headers,"Cache-Control: max-age=0");
    headers = curl_slist_append(headers,"Content-Type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers,"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
    headers = curl_slist_append(headers,"Referer:http://ddc.zhangh.com/Citrix/StoreWeb/");
    headers = curl_slist_append(headers,"Accept-Encoding: gzip, deflate");
    headers = curl_slist_append(headers,"Accept-Language: zh-CN,zh;q=0.8");
//  headers = curl_slist_append(headers,"Cookie: olfsk=olfsk7257964002188215; hblid=nR2Zo2hcfSVtItpT3m39N804DHbNAfAp; pass=password");//也可以直接通过头部提交cookie
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_COOKIEFILE,"");//CURLOPT_COOKIEFILE参数用于从文件读取cookie。这里用于初始化cookie引擎,这样后面的curl_easy_getinfo才能正确接收到cookie数据.
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookie_open.txt");//保存从服务器返回的cookie到文件，也就是说我们自己设置的并不会保存
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, "http://ddc.zhangh.com/Citrix/StoreWeb/Home/Configuration");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, function);
    curl_easy_setopt(curl, CURLOPT_POST, 1);//设置CURLOPT_POST之后必须带有POST数据
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "pass=23333&submit=%CC%E1%BD%BB");
    curl_easy_setopt(curl, CURLOPT_COOKIE, "olfsk=olfsk7257964002188215; hblid=nR2Zo2hcfSVtItpT3m39N804DHbNAfAp; pass=adadad");

    curl_easy_perform(curl);

    curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);//获取cookie，只获取服务器返回的cookie
    cookie = cookies;
    while(cookie)
    {
        printf("%s\n", cookie->data);
        cookie = cookie->next;
    }

    curl_slist_free_all(headers);
    curl_slist_free_all(cookies);
    curl_easy_cleanup(curl);
    getchar();
    return 0;    
}
