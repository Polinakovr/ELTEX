#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int main()
{
    pid_t branch_bash;
    int pipefd[2];
    int pipe(pipefd);
    char buffer[30];
    char *argv[10];
    char *saveptr;
    while (1)
    {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            if (feof(stdin))
            {
                exit(0);
            }
            else
            {
                perror("Ошибка чтения ввода");
                continue;
            }
        }
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strcmp(buffer, "exit") == 0)
        {
            exit(0);
        }

        char *token = strtok_r(buffer, " ", &saveptr);

        int i = 0;
        while (token != NULL && i < 9)
        {
            argv[i] = token;
            i++;
            token = strtok_r(NULL, " ", &saveptr);
        }
        argv[i] = NULL;
        char buf[30] = "/bin/";
        char *path = strcat(buf, argv[0]);
        branch_bash = fork();

        switch (branch_bash)
        {
            case -1:
                perror("Ошибка дочернего процесса");
                exit(1);
                break;
            default: wait(NULL); break;
            case 0:
                if (execv(path, argv) == -1)
                { /*Результат записать в канала,  в массив ls , и с помощью канала переправить на grep и чтобы он тоже, и еще dup2 подменить вывод (посмотреть лецию) */
                    perror("Ошибка выполнения команды");
                    exit(2);
                    break;
                }
                exit(0);
                break;
        }
    }
    return 0;
}