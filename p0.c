#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main()
{
   int fd_prod;
   fd_prod= open("/dev/cons0",O_RDWR);
   if(fd_prod<0)
   {
	   printf("FAILED TO OPEN CONSUMER FILE!!!\n");
           return fd_prod;
   }
   write(fd_prod,"satya",6);

   return 0;
}


