#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main()
{
    sigset_t sa;
    int sig;
    sigaddset(&sa, SIGUSR1);
    pid_t pid = getpid();
    printf("%d\n", pid);
    sigprocmask(SIG_BLOCK, &sa, NULL);
    while (1)
    {
        printf("Ждем сигнал\n");
        if (sigwait(&sa, &sig) == 0)
        {
            printf("Сигнал получен %d \n", sig);
        }
    }
    return 0;
}