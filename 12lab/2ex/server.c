#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
int main()
{
    char *fifo_path = "/tmp/fifo02";
    int des;
    unlink(fifo_path);

    if (mkfifo(fifo_path, 0666) == -1)
    {
        perror("Ошибка создания");
        unlink(fifo_path);
        exit(EXIT_FAILURE);
    }
    if ((des = open(fifo_path, O_WRONLY)) == -1)
    {
        perror("Ошибка открытия");
        unlink(fifo_path);
        exit(EXIT_FAILURE);
    }
    if (write(des, "Hi!", 4) == -1)
    {
        perror("Ошибка записи");
        close(des);
        unlink(fifo_path);
        exit(EXIT_FAILURE);
    }
    close(des);
}