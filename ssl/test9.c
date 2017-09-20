#include<stdio.h>
int main()
{
   int i=0;
   char c;
   char str[30];
   printf("input a int  :");
   scanf("%d",&i);
   printf("input a char :");
   scanf("%c",&c);
   printf("input string :");
   scanf("%s",str);
   printf("int is %d,\tchar is %c,\tstring is %s\n",i,c,str);
   return 0;
}
