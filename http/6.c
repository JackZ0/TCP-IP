#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

#define HOST_NAME_LEN   256
#define URI_MAX_LEN     2048
#define RECV_BUF        8192
#define RCV_SND_TIMEOUT (10*1000)   //收发数据超时时间(ms)

typedef struct {
    int sock;                       //与服务器通信的socket
    FILE *in;                       //sock描述符转为文件指针，方便读写
    char host_name[HOST_NAME_LEN];  //主机名
    int port;                       //主机端口号
    char uri[URI_MAX_LEN];          //资源路径
    char buffer[RECV_BUF];          //读写缓冲
    int status_code;                //http状态码
    int chunked_flag;               //chunked传输的标志位
    int len;                        //Content-length里的长度
    char location[URI_MAX_LEN];     //重定向地址
    char *save_path;                //保存内容的路径指针
    FILE *save_file;                //保存内容的文件指针
    int recv_data_len;              //收到数据的总长度
    time_t start_recv_time;         //开始接受数据的时间
    time_t end_recv_time;           //结束接受数据的时间
} http_t;

/* 打印宏 */
#define MSG_DEBUG   0x01
#define MSG_INFO    0x02
#define MSG_ERROR   0x04

static int print_level = /*MSG_DEBUG |*/ MSG_INFO | MSG_ERROR;

#define lprintf(level, format, argv...) do{     \
    if(level & print_level)     \
        printf("[%s][%s(%d)]:"format, #level, __FUNCTION__, __LINE__, ##argv);  \
}while(0)

#define MIN(x, y) ((x) > (y) ? (y) : (x))

#define HTTP_OK         200
#define HTTP_REDIRECT   302
#define HTTP_NOT_FOUND  404

/* 不区分大小写的strstr */
char *strncasestr(char *str, char *sub)
{
    if(!str || !sub)
        return NULL;

    int len = strlen(sub);
    if (len == 0)
    {
        return NULL;
    }

    while (*str)
    {
        if (strncasecmp(str, sub, len) == 0)
        {
            return str;
        }
        ++str;
    }
    return NULL;
}

/* 解析URL, 成功返回0，失败返回-1 */
/* http://127.0.0.1:8080/testfile */
int parser_URL(char *url, http_t *info)
{
    char *tmp = url, *start = NULL, *end = NULL;
    int len = 0;

    /* 跳过http:// */
    if(strncasestr(tmp, "http://"))
    {   
        tmp += strlen("http://");
    }
    start = tmp;
    if(!(tmp = strchr(start, '/')))
    {
        lprintf(MSG_ERROR, "url invaild\n");
        return -1;      
    }
    end = tmp;

    /*解析端口号和主机*/
    info->port = 443;   //先附默认值80

    len = MIN(end - start, HOST_NAME_LEN - 1);
    strncpy(info->host_name, start, len);
    info->host_name[len] = '\0';

    if((tmp = strchr(start, ':')) && tmp < end)
    {
        info->port = atoi(tmp + 1);
        if(info->port <= 0 || info->port >= 65535)
        {
            lprintf(MSG_ERROR, "url port invaild\n");
            return -1;
        }
        /* 覆盖之前的赋值 */
        len = MIN(tmp - start, HOST_NAME_LEN - 1);
        strncpy(info->host_name, start, len);
        info->host_name[len] = '\0';
    }

    /* 复制uri */
    start = end;
    strncpy(info->uri, start, URI_MAX_LEN - 1);

    lprintf(MSG_INFO, "parse url ok\nhost:%s, port:%d, uri:%s\n", 
        info->host_name, info->port, info->uri);
    return 0;
}

/* dns解析,返回解析到的第一个地址，失败返回-1，成功则返回相应地址 */
unsigned long dns(char* host_name)
{

    struct hostent* host;
    struct in_addr addr;
    char **pp;

    host = gethostbyname(host_name);
    if (host == NULL)
    {
        lprintf(MSG_ERROR, "gethostbyname %s failed\n", host_name);
        return -1;
    }

    pp = host->h_addr_list;

    if (*pp!=NULL)
    {
        addr.s_addr = *((unsigned int *)*pp);
        lprintf(MSG_INFO, "%s address is %s\n", host_name, inet_ntoa(addr));
        pp++;
        return addr.s_addr;
    }

    return -1;
}

/* 设置发送接收超时 */
int set_socket_option(int sock)
{
    struct timeval timeout;

    timeout.tv_sec = RCV_SND_TIMEOUT/1000;
    timeout.tv_usec = RCV_SND_TIMEOUT%1000*1000;
    lprintf(MSG_DEBUG, "%ds %dus\n", (int)timeout.tv_sec, (int)timeout.tv_usec);
    //设置socket为非阻塞
    // fcntl(sock ,F_SETFL, O_NONBLOCK); //以非阻塞的方式，connect需要重新处理

    // 设置发送超时
    if(-1 == setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, 
            sizeof(struct timeval)))
    {
        lprintf(MSG_ERROR, "setsockopt error: %m\n");
        return -1;
    }

    // 设置接送超时
    if(-1 == setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, 
            sizeof(struct timeval)))
    {
        lprintf(MSG_ERROR, "setsockopt error: %m\n");
        return -1;
    }

    return 0;
}

/* 连接到服务器 */
int connect_server(http_t *info)
{
    int sockfd;
    struct sockaddr_in server;
    unsigned long addr = 0;
    unsigned short port = info->port;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
        lprintf(MSG_ERROR, "socket create failed\n");
        goto failed;
    }

    if(-1 == set_socket_option(sockfd))
    {
        goto failed;
    }

    if ((addr = dns(info->host_name)) == -1)
    {
        lprintf(MSG_ERROR, "Get Dns Failed\n");
        goto failed;
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET; 
    server.sin_port = htons(port); 
    server.sin_addr.s_addr = addr;

    if (-1 == connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr)))
    {
        lprintf(MSG_ERROR, "connect failed: %m\n");
        goto failed;
    }

    info->sock = sockfd;
    return 0;

failed:
    if(sockfd != -1)
        close(sockfd);
    return -1;
}

/* 发送http请求 */
int send_request(http_t *info)
{
    int len;

    memset(info->buffer, 0x0, RECV_BUF);
    snprintf(info->buffer, RECV_BUF - 1, "GET %s HTTP/1.1\r\n"
        "Accept: */*\r\n"
        "User-Agent: Mozilla/5.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\n"
        "Host: %s\r\n"
        "Connection: Close\r\n\r\n", info->uri, info->host_name);

    lprintf(MSG_DEBUG, "request:\n%s\n", info->buffer);
    return send(info->sock, info->buffer, strlen(info->buffer), 0);
}

/* 解析http头 */
int parse_http_header(http_t *info)
{
    char *p = NULL;

    // 解析第一行
    fgets(info->buffer, RECV_BUF, info->in);
    p = strchr(info->buffer, ' ');
    //简单检查http头第一行是否合法
    if(!p || !strcasestr(info->buffer, "HTTP"))
    {
        lprintf(MSG_ERROR, "bad http head\n");
        return -1;
    }
    info->status_code = atoi(p + 1);   
    lprintf(MSG_DEBUG, "http status code: %d\n", info->status_code);

    // 循环读取解析http头
    while(fgets(info->buffer, RECV_BUF, info->in))
    {
        // 判断头部是否读完
        if(!strcmp(info->buffer, "\r\n"))
        {
            return 0;   /* 头解析正常 */
        }
        lprintf(MSG_DEBUG, "%s", info->buffer);
        // 解析长度 Content-length: 554
        if(p = strncasestr(info->buffer, "Content-length"))
        {
            p = strchr(p, ':');
            p += 2;     // 跳过冒号和后面的空格
            info->len = atoi(p);
            lprintf(MSG_INFO, "Content-length: %d\n", info->len);
        }
        else if(p = strncasestr(info->buffer, "Transfer-Encoding"))
        {
            if(strncasestr(info->buffer, "chunked"))
            {
                info->chunked_flag = 1;
            }
            else
            {
                /* 不支持其他编码的传送方式 */
                lprintf(MSG_ERROR, "Not support %s", info->buffer);
                return -1;
            }
            lprintf(MSG_INFO, "%s", info->buffer);
        }
        else if(p = strncasestr(info->buffer, "Location"))
        {
            p = strchr(p, ':');
            p += 2;     // 跳过冒号和后面的空格
            strncpy(info->location, p, URI_MAX_LEN - 1);
            lprintf(MSG_INFO, "Location: %s\n", info->location);
        }
    }
    lprintf(MSG_ERROR, "bad http head\n");
    return -1;  /* 头解析出错 */
}

/* 保存服务器响应的内容 */
int save_data(http_t *info, const char *buf, int len)
{
    int total_len = len;
    int write_len = 0;

    // 文件没有打开则先打开
    if(!info->save_file)
    {
        info->save_file = fopen(info->save_path, "w");
        if(!info->save_file)
        {
            lprintf(MSG_ERROR, "fopen %s error: %m\n", info->save_path);
            return -1;
        }
    }

    while(total_len)
    {
        write_len = fwrite(buf, sizeof(char), len, info->save_file);
        if(write_len < len && errno != EINTR)
        {
            lprintf(MSG_ERROR, "fwrite error: %m\n");
            return -1;
        }
        total_len -= write_len;
    }
}

/* 读数据 */
int read_data(http_t *info, int len)
{
    int total_len = len;
    int read_len = 0;
    int rtn_len = 0;

    while(total_len)
    {
        read_len = MIN(total_len, RECV_BUF);
        // lprintf(MSG_DEBUG, "need read len: %d\n", read_len);
        rtn_len = fread(info->buffer, sizeof(char), read_len, info->in);
        if(rtn_len < read_len)
        {
            if(ferror(info->in))
            {
                if(errno == EINTR) /* 信号中断了读操作 */
                {
                    ;   /* 不做处理继续往下走 */
                }
                else if(errno == EAGAIN || errno == EWOULDBLOCK) /* 超时 */
                {
                    lprintf(MSG_ERROR, "socket recvice timeout: %dms\n", RCV_SND_TIMEOUT);
                    total_len -= rtn_len;
                    lprintf(MSG_DEBUG, "read len: %d\n", rtn_len);
                    break;
                }
                else    /* 其他错误 */
                {
                    lprintf(MSG_ERROR, "fread error: %m\n");
                    break;
                }
            }
            else    /* 读到文件尾 */
            {
                lprintf(MSG_ERROR, "socket closed by peer\n");
                total_len -= rtn_len;
                lprintf(MSG_DEBUG, "read len: %d\n", rtn_len);
                break;
            }
        }

        // lprintf(MSG_DEBUG, " %s\n", info->buffer);
        total_len -= rtn_len;
        lprintf(MSG_DEBUG, "read len: %d\n", rtn_len);
        if(-1 == save_data(info, info->buffer, rtn_len))
        {
            return -1;
        }
        info->recv_data_len += rtn_len;
    }
    if(total_len != 0)
    {
        lprintf(MSG_ERROR, "we need to read %d bytes, but read %d bytes now\n", 
            len, len - total_len);
        return -1;
    }
}

/* 接收服务器发回的chunked数据 */
int recv_chunked_response(http_t *info)
{
    long part_len;

    //有chunked，content length就没有了
    do{
        // 获取这一个部分的长度
        fgets(info->buffer, RECV_BUF, info->in);
        part_len = strtol(info->buffer, NULL, 16);
        lprintf(MSG_DEBUG, "part len: %ld\n", part_len);
        if(-1 == read_data(info, part_len))
            return -1;

        //读走后面的\r\n两个字符
        if(2 != fread(info->buffer, sizeof(char), 2, info->in))
        {
            lprintf(MSG_ERROR, "fread \\r\\n error : %m\n");
            return -1;
        }
    }while(part_len);
    return 0;
}

/* 计算平均下载速度，单位byte/s */
float calc_download_speed(http_t *info)
{
    int diff_time = 0; 
    float speed = 0.0;

    diff_time = info->end_recv_time - info->start_recv_time;
    /* 最小间隔1s，避免计算浮点数结果为inf */
    if(0 == diff_time)
        diff_time = 1;
    speed = (float)info->recv_data_len / diff_time;

    return  speed;
}

/* 接收服务器的响应数据 */
int recv_response(http_t *info)
{
    int len = 0, total_len = info->len;

    if(info->chunked_flag)
        return recv_chunked_response(info);

    if(-1 == read_data(info, total_len))
        return -1;

    return 0;
}

/* 清理操作 */
void clean_up(http_t *info)
{
    if(info->in)
        fclose(info->in);
    if(-1 != info->sock)
        close(info->sock);
    if(info->save_file)
        fclose(info->save_file);
    if(info)
        free(info);
}

/* 下载主函数 */
int http_download(char *url, char *save_path)
{
    http_t *info = NULL;
    char tmp[URI_MAX_LEN] = {0};

    if(!url || !save_path)
        return -1;

    //初始化结构体
    info = malloc(sizeof(http_t));
    if(!info)
    {
        lprintf(MSG_ERROR, "malloc failed\n");
        return -1;
    }
    memset(info, 0x0, sizeof(http_t));
    info->sock = -1;
    info->save_path = save_path;

    // 解析url
    if(-1 == parser_URL(url, info))
        goto failed;

    // 连接到server
    if(-1 == connect_server(info))
        goto failed;

    // 发送http请求报文
    if(-1 == send_request(info))
        goto failed;

    // 接收响应的头信息
    info->in = fdopen(info->sock, "r");
    if(!info->in)
    {
        lprintf(MSG_ERROR, "fdopen error\n");
        goto failed;
    }

    // 解析头部
    if(-1 == parse_http_header(info))
        goto failed;

    switch(info->status_code)
    {
        case HTTP_OK:
            // 接收数据
            lprintf(MSG_DEBUG, "recv data now\n");
            info->start_recv_time = time(0);
            if(-1 == recv_response(info))
                goto failed;

            info->end_recv_time = time(0);
            lprintf(MSG_INFO, "recv %d bytes\n", info->recv_data_len);
            lprintf(MSG_INFO, "Average download speed: %.2fKB/s\n", 
                    calc_download_speed(info)/1000);
            break;
        case HTTP_REDIRECT:
            // 重启本函数
            lprintf(MSG_INFO, "redirect: %s\n", info->location);
            strncpy(tmp, info->location, URI_MAX_LEN - 1);
            clean_up(info);
            return http_download(tmp, save_path);

        case HTTP_NOT_FOUND:
            // 退出
            lprintf(MSG_ERROR, "Page not found\n");
            goto failed;
            break;

        default:
            lprintf(MSG_INFO, "Not supported http code %d\n", info->status_code);
            goto failed;
    }

    clean_up(info);
    return 0;
failed:
    clean_up(info);
    return -1;
}

/****************************************************************************
测试用例:
(1)chunked接收测试
./a.out "http://www.httpwatch.com/httpgallery/chunked/chunkedimage.aspx" test.aspx
(2)重定向测试
./a.out "192.168.10.1/main.html" test.txt
(3)错误输入测试
./a.out "32131233" test.txt
(4)根目录输入测试
./a.out "www.baidu.com/" test.txt
(5)端口号访问测试
./a.out "192.168.0.200:8000/FS_AC6V1.0BR_V15.03.4.12_multi_TD01.bin" test.txt
****************************************************************************/

int main(int argc, char *argv[])
{
    if(argc < 3)
        return -1;

    http_download(argv[1], argv[2]);
    return 0;
}
