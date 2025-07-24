#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    while (1)
    {
        char buffer[30];
        char path1[15];
        char path2[15];
        char *argv[2][4]={{0}};
        int pipefd[2];
        char *saveptr;
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            if (feof(stdin))
            {
                exit(EXIT_SUCCESS);
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
        argv[0][0]=token;
        snprintf(path1, 15, "/bin/%s", token);
        int i = 1;
        while ((token = strtok_r(NULL, " ", &saveptr)) != NULL && strcmp(token, "|") != 0 && i < 4)
        {
            argv[0][i++] = token;
        }
        argv[0][i] = NULL;

        if (token != NULL && strcmp(token, "|") == 0)
        {
            if (pipe(pipefd) == -1)
            {
                perror("Ошибка создания неименовоного канала");
                exit(EXIT_FAILURE);
            }
            token = strtok_r(NULL, " ", &saveptr);
            if (token == NULL)
                continue;

            snprintf(path2, sizeof(path2), "/bin/%s", token);
            argv[1][0] = token;

            i = 1;
            while ((token = strtok_r(NULL, " ", &saveptr)) != NULL && i < 4)
            {
                argv[1][i++] = token;
            }
            argv[1][i] = NULL;
            
            pid_t branch1 = fork();
            if (branch1 == -1)
            {
                perror("Ошибка дочернего процесса");
                close(pipefd[0]);
                close(pipefd[1]);
                continue;
            }

            if (branch1 == 0)
            {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
                execv(path1, argv[0]);
                perror("Ошибка выполнения команды");
                exit(EXIT_FAILURE); 
            }

            pid_t branch2 = fork();
            if (branch2 == -1)
            {
                perror("Ошибка дочернего (2) процесса");
                close(pipefd[0]);
                close(pipefd[1]);
                waitpid(branch1, NULL, 0);
                continue;
            }

            if (branch2 == 0)
            {
            
                close(pipefd[1]);
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);
                execv(path2, argv[1]);
                perror("Ошибка выполнения команды");
                exit(EXIT_FAILURE);  
            }

            close(pipefd[0]);
            close(pipefd[1]);
            waitpid(branch1, NULL, 0);
            waitpid(branch2, NULL, 0);
        }
        else
        {
            pid_t branch_pid = fork();
            switch (branch_pid)
            {
                case -1:
                    perror("Ошибка дочернего процесса");
                    exit(1);
                    break;
                default: waitpid(branch_pid, NULL, 0); break;
                case 0:
                    if (execv(path1, argv[0]) == -1)
                    {
                        perror("Ошибка выполнения команды");
                        exit(2);
                        break;
                    }
                    exit(0);
                    break;
            }
        }
    }
    return 0;
} 