#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

void cpJSC(char origen[], char destino[])
{
    int fd_origen = open(origen, O_RDONLY);
    if (fd_origen < 0)
    {
        perror("Error, no hay nada en el origen");
        return;
    }

    int fd_destino = open(destino, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_destino < 0)
    {
        perror("Error, no se creo nada");
        close(fd_origen);
        return;
    }

    char buffer[1024];
    ssize_t bytes;

    while ((bytes = read(fd_origen, buffer, sizeof(buffer))) > 0)
    {
        write(fd_destino, buffer, bytes);
    }

    close(fd_origen);
    close(fd_destino);
}