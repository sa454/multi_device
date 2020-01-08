#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SIZE 100
unsigned char conbuff[SIZE];
int main()
{
   int fd_con;
   fd_con = open("/dev/test0",O_RDWR);
   if(fd_con<0)
   {
           printf("FAILED TO OPEN CONSUMER FILE!!!\n");
           return fd_con;
   }
    read(fd_con,conbuff,100);
    printf("date found:\n%s",conbuff);
   return 0;
}


