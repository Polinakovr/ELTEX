#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int main()
{
    pid_t children;
    int status;
    children = fork();
    switch (children)
    {
        case -1:
            perror("Ошибка создания дочернего процесса");
            exit(1);
            break;
        case 0: printf("PID children=%d PPID children=%d \n", getpid(), getppid()); break;
        default:
            printf("PID %d PPID =%d\n", getpid(), getppid());
            wait(&status);
            printf("status=%d", WEXITSTATUS(status));
            break;
    }
    return 0;
}