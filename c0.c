#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SIZE 1000 
unsigned char conbuff[SIZE];
int main()
{
   int fd_con;
   fd_con = open("/dev/cons0",O_RDWR);
  
   if(fd_con<0)
   {
           printf("FAILED TO OPEN CONSUMER FILE!!!\n");
           return fd_con;
   }
    read(fd_con,conbuff,SIZE);
    printf("date found:%s",conbuff);
   return 0;
}


