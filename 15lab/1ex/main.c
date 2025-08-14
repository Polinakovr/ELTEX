#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void handler(int sig)
{
    printf("Сигнал был получен\n");
}
int main()
{
    struct sigaction sa;
    sa.sa_handler = &handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=0;
    pid_t pid = getpid();
    printf("%d\n", pid);
    sigaction(SIGUSR1, &sa, NULL);
    while (1)
    {
        sleep(1);
    }
}