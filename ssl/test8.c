#include <string>   
#include <iomanip>  
#include <iostream>   
#include <openssl/md5.h>  
  
using namespace std;  
  
#ifdef WIN32  
#pragma comment(lib, "libeay32.lib")  
#pragma comment(lib, "ssleay32.lib")  
#endif  
  
int main(int argc, char* argv[])  
{  
    std::string str = "1234";  
    unsigned char md[MD5_DIGEST_LENGTH];  
    char tmp[3] = { 0 };  
  
    MD5((const unsigned char*)str.c_str(), str.size(), md);  
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)  
    {  
        cout<<setw(2)<<setfill('0')<<hex<<(int)(md[i]);  
    }  
    cout << endl;  
    return 0;  
}  
