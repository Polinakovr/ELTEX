#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
void forkprint(pid_t fork, int count)
{
    switch (fork)
    {
        case -1:
            perror("Ошибка создания дочернего процесса");
            exit(2);
            break;
        default: printf("\n PID children=%d PPID children=%d, proc=%d \n", getpid(), getppid(), count); break;
    }
}
int main()
{
    pid_t proc;
    proc = fork();
    switch (proc)
    {
        case 0:
            forkprint(proc, 1);
            proc = fork();

            if (proc == 0)
            {
                forkprint(proc, 3);
                exit(3);
            }
            proc = fork();
            if (proc == 0)
            {
                forkprint(proc, 4);
                exit(4);
            }

            wait(NULL);
            wait(NULL);
            exit(1);
            break;

        case -1:
            perror("Ошибка создания дочернего процесса (1)");
            exit(2);
            break;
        default:
            proc = fork();

            if (proc == 0)
            {
                forkprint(proc, 2);
                proc = fork();
                if (proc == 0)
                {
                    forkprint(proc, 5);
                }
                wait(NULL);
                exit(5);
            }
            wait(NULL);
            wait(NULL);
            exit(0);
            break;
    }

    return 0;
}