#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        perror("Error parameters");
        return 1;
    }
    pid_t pid=atoi(argv[1]);
    kill(pid, SIGUSR1);
    return 0;
}