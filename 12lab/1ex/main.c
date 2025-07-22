#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
int main()
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("Ошибка создания неименовоного канала");
        exit(EXIT_FAILURE);
    }
    pid_t child = fork();
    switch (child)
    {
        case -1:
            perror("Ошибка создания дочернего процесса");
            exit(EXIT_FAILURE);
            break;
        case 0:
            char buffer[5];
            close(pipefd[1]);
            if (read(pipefd[0], &buffer, sizeof(buffer)) == -1)
            {
                perror("Ошибка чтения");
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < sizeof(buffer); i++)
            {
                printf("%c", buffer[i]);
            }
            close(pipefd[0]);
            break;

        default:
            close(pipefd[0]);
            if (write(pipefd[1], "Hi!", 4) == -1)
            {
                perror("Ошибка записи");
                wait(NULL);
                exit(EXIT_FAILURE);
            }
            wait(NULL);
            close(pipefd[1]);
            break;
    }
}