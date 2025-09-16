#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define SIZE 65536
int socketfd;
void handler(int sig)
{
    close(socketfd);
    printf("Clean");
    exit(EXIT_SUCCESS);
}
int main()
{
    struct sigaction s;
    s.sa_handler = &handler;
    sigaction(SIGINT, &s, NULL);
    if ((socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1)
    {
        printf("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        unsigned char buffer[SIZE];
        if ((recvfrom(socketfd, buffer, sizeof(buffer), 0, NULL, NULL)) == -1)
        {
            if (errno = EINTR)
            {
                printf("EINTR:%s", strerror(errno));
            }
            else
            {
                printf("%s", strerror(errno));
                close(socketfd);
                exit(EXIT_FAILURE);
            }
        }
        for (int i = 0; i < SIZE; i++)
        {
            printf("%02x", buffer[i]);
             if ((i + 1) % 16 == 0) printf("\n");
        }
           printf("\n===============================\n");
    }
}