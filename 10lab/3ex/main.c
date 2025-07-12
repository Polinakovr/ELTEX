#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int main()
{
    pid_t branch_bash;

    char buffer[30];
    char *argv[10];
    while (1)
    {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strcmp(buffer, "exit") == 0)
        {
            exit(0);
        }

        char *token = strtok(buffer, " ");

        int i = 0;
        while (token != NULL)
        {
            argv[i] = token;
            i++;
            token = strtok(NULL, " ");
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
                {
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