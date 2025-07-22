#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
int main()
{
    int des;
    char buffer[5];
    if ((des = open("/tmp/fifo02", O_RDONLY)) == -1)
    {
        perror("Ошибка открытия со стороны клиента");
        unlink("/tmp/fifo02");
        exit(EXIT_FAILURE);
    }
    if (read(des, buffer, 4) == -1)
    {
        perror("Ошибка чтения со стороны клиента");
        close(des);
        unlink("/tmp/fifo02");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < sizeof(buffer); i++)
    {
        printf("%c", buffer[i]);
    }
    close(des);
    unlink("/tmp/fifo02");
}