#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "catJSC.h"

void catJSC(char texto[])
{
   int fd = open(texto, O_RDWR | O_CREAT, 0644);
   char buffer[1024];
   ssize_t bytes;
   struct stat fileStat;

   printf("v\n");

   if ((bytes = read(fd, buffer, sizeof(buffer))) == 0)
   {
      close(fd);
      fgets(buffer, sizeof(buffer), stdin);
      fd = open(texto, O_WRONLY | O_APPEND, 0644);
      char ultimo = buffer[strlen(buffer) - 1];
      char char_final = '$';
      while (ultimo != char_final)
      {
         write(fd, buffer, strlen(buffer));
         fgets(buffer, sizeof(buffer), stdin);
         ultimo = buffer[strlen(buffer) - 2];
      }
      buffer[strlen(buffer) - 1] = '\0';
      buffer[strlen(buffer) - 1] = '\0';
      printf("\n\n----%s----\n\n", buffer);
      write(fd, buffer, strlen(buffer));
      close(fd);
   }
   else
   {
      close(fd);
      fd = open(texto, O_RDONLY, 0644);
      if (stat(texto, &fileStat) == 0)
      {
         bytes = read(fd, buffer, sizeof(buffer) - 1);
         if (bytes > 0)
         {
            buffer[bytes] = '\0';
            printf("\n\n%s\n\n", buffer);
         }
      }
   }
   return;
}
