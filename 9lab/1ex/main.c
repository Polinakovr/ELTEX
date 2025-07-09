#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
int main()
{
    int f = open("output.txt", O_RDWR);
    char *buffer = "String from file";
    int size = strlen(buffer);
    if (f == -1)
    {
        perror("Ошибка открытия файла");
        exit(-1);
    }
    ssize_t w = write(f, buffer, size);
    if (w == -1)
    {
        perror("Ошибка записи файла");
        close(f);
        exit(-1);
    }
    char *buffer2 = malloc(size + 1);

    off_t p = lseek(f, -size, SEEK_END);
    if (p == -1)
    {
        perror("Ошибка перемещения");
        free(buffer2);
        close(f);
        exit(-1);
    }
    ssize_t r = read(f, buffer2, size);
    if (r == -1)
    {
        perror("Ошибка чтения файла");
        free(buffer2);
        close(f);
        exit(-1);
    }
    buffer2[r] = '\0';
    printf("%s", buffer2);

    close(f);
    free(buffer2);
}