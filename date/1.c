#include <stdio.h>  
#include <stdlib.h>  
  
char *weaks[] = { "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday" };  
  
/** 
 * 基姆拉尔森计算星期几 
 */  
void KimLarsson(int year, int month, int day) {  
    int w = 0;  
    //将一月和二月当成前一年的13月和14月份计算  
    if (month == 1 || month == 2)  
    {  
        year--;  
        month += 12;  
    }  
    //基姆拉尔森计算公式  
    w = (day + 2*month + 3*(month + 1)/5 + year + year / 4 -year / 100 + year / 400 + 1) % 7;  
    //输出星期几  
    printf("%s\n",weaks[w]);  
}  
  
int main(int argc, char *argv[]) {  
  
    int y;          //年  
    int m;          //月  
    int d;          //日  
      
    while (1)  
    {  
        //输入年月日，以空格隔开。例如：1990 1 1  
        scanf("%d %d %d", &y, &m, &d);  
        //调用基姆拉尔森计算星期几  
        KimLarsson(y, m, d);  
    }  
    return 0;  
}  
