#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main()
{
    sigset_t sa;

    sigaddset(&sa, SIGINT);
    pid_t pid = getpid();
    printf("%d\n", pid);
    if (sigprocmask(SIG_BLOCK, &sa, NULL) == -1)
    {
        perror("Sigprocmask error");
    }
      printf("Сигнал SIGINT заблокирован. Уход в бесконечный цикл...\n");

    while (1)
    {
     sleep(1);
    }
}